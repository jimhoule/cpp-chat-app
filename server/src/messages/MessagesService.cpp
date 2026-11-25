#include "messages/MessagesService.h"

#include "conversations/ConversationsService.h"
#include "log/Logger.h"
#include "uuid/UuidService.h"
#include "views/ConversationUserView.h"

#include <cstddef>
#include <cstdint>
#include <utility>

constexpr std::size_t MAX_MESSAGE_TEXT_LENGTH = 4000;
constexpr std::size_t MAX_MESSAGES_PAGE_ITEMS = 42;

// **********
// * PUBLIC *
// **********
MessagesService::MessagesService(std::unique_ptr<IMessagesRepository> messagesRepository, ConversationsService& conversationsService, UuidService& uuidService, Logger& logger)
    : m_messagesRepository(std::move(messagesRepository))
    , m_conversationsService(conversationsService)
    , m_uuidService(uuidService)
    , m_logger(logger)
{}

MessagesService::CreateMessageResult MessagesService::Create(const CreateMessageDto& createMessageDto)
{
    CreateMessageResult createMessageResult = {};

    // NOTE: Only a user of the conversation can post in it
    const bool isConversationUser = IsConversationUser(createMessageDto.conversationId, createMessageDto.senderId);
    if (!isConversationUser)
    {
        createMessageResult.code = MessagesResultCode::NOT_CONVERSATION_USER;
        return createMessageResult;
    }

    // Validates text
    if (createMessageDto.text.empty())
    {
        createMessageResult.code = MessagesResultCode::TEXT_EMPTY;
        return createMessageResult;
    }

    if (createMessageDto.text.size() > MAX_MESSAGE_TEXT_LENGTH)
    {
        createMessageResult.code = MessagesResultCode::TEXT_TOO_LONG;
        return createMessageResult;
    }

    // Creates message
    Message message = {};
    message.id = m_uuidService.Generate();
    message.conversationId = createMessageDto.conversationId;
    message.senderId = createMessageDto.senderId;
    message.text = createMessageDto.text;
    message.createdAt = std::time(0);

    MessageView messageView = ConvertMessageToMessageView(m_messagesRepository->Create(message));

    // NOTE: Logs the text length, never the text itself
    m_logger.Info("Created message " + message.id + " in conversation " + message.conversationId + " from user " + message.senderId + " (" + std::to_string(message.text.size()) + " chars)");

    /**
     * NOTES:
     *  - Fetches conversation before reopening so it still tells who had it closed
     *  - Membership (conversation user) check above proved the conversation exists so value is safe
     */
    ConversationsService::FindConversationByIdDto findConversationByIdDto = {};
    findConversationByIdDto.id = messageView.conversationId;
    const ConversationsService::ConversationResult conversationResult = m_conversationsService.FindById(findConversationByIdDto);

    CreateMessageData createMessageData = {};
    createMessageData.message = std::move(messageView);

    // Collects users who had close the conversation
    for (const ConversationUserView& conversationUser : conversationResult.data.value().users)
    {
        if (!conversationUser.isOpen)
        {
            createMessageData.reopenedUserIds.push_back(conversationUser.user.id);
        }
    }

    // Reopens conversation for users who had closed it
    ConversationsService::OpenConversationForAllUsersDto openConversationForAllUsersDto = {};
    openConversationForAllUsersDto.id = message.conversationId;
    ConversationsService::ConversationResult openedConversationResult = m_conversationsService.OpenForAllUsers(openConversationForAllUsersDto);

    createMessageData.conversation = std::move(openedConversationResult.data.value());
    createMessageResult.data = std::move(createMessageData);

    return createMessageResult;
}

MessagesService::MessagesReadStatesResult MessagesService::FindAllOpenReadStatesByUserId(const FindAllOpenMessagesReadStatesByUserIdDto& findAllOpenMessagesReadStatesByUserIdDto)
{
    // Finds where user stopped reading in every open conversation
    ConversationsService::FindAllOpenLastReadMessagePositionsByUserIdDto findAllOpenLastReadMessagePositionsByUserIdDto = {};
    findAllOpenLastReadMessagePositionsByUserIdDto.userId = findAllOpenMessagesReadStatesByUserIdDto.userId;
    const ConversationsService::LastReadMessagePositionsResult lastReadMessagePositionsResult = m_conversationsService.FindAllOpenLastReadMessagePositionsByUserId(findAllOpenLastReadMessagePositionsByUserIdDto);

    const IConversationsRepository::LastReadMessagePositionsMap& lastReadMessagePositionsMap = lastReadMessagePositionsResult.data;

    // Counts unread messages after each position and finds newest message of each conversation (one pass for all conversations)
    const IMessagesRepository::MessagesCountsMap messagesCountsMap = m_messagesRepository->CountByConversationIdsAfter(
        lastReadMessagePositionsMap,
        findAllOpenMessagesReadStatesByUserIdDto.userId
    );

    // Builds one read state per conversation (messagesCountsMap has an entry for every conversation)
    std::vector<MessagesReadStateView> messagesReadStateViews = {};
    messagesReadStateViews.reserve(lastReadMessagePositionsMap.size());
    for (const IConversationsRepository::LastReadMessagePositionsMap::value_type& lastReadMessagePositionPair : lastReadMessagePositionsMap)
    {
        const IMessagesRepository::MessagesCount& messagesCount = messagesCountsMap.at(lastReadMessagePositionPair.first);

        MessagesReadStateView messagesReadStateView = {};
        messagesReadStateView.conversationId = lastReadMessagePositionPair.first;
        messagesReadStateView.lastReadMessagePosition = lastReadMessagePositionPair.second;
        messagesReadStateView.latestMessagePosition = messagesCount.latestMessagePosition;
        messagesReadStateView.unreadCount = messagesCount.count;

        messagesReadStateViews.push_back(std::move(messagesReadStateView));
    }

    MessagesReadStatesResult messagesReadStatesResult = {};
    messagesReadStatesResult.data = std::move(messagesReadStateViews);

    return messagesReadStatesResult;
}

MessagesService::MessagesPageResult MessagesService::FindPageByConversationId(const FindMessagesPageByConversationIdDto& findMessagesPageByConversationIdDto)
{
    MessagesPageResult messagesPageResult = {};

    // NOTE: Only a user of the conversation can fetch messages
    const bool isConversationUser = IsConversationUser(findMessagesPageByConversationIdDto.conversationId, findMessagesPageByConversationIdDto.userId);
    if (!isConversationUser)
    {
        messagesPageResult.code = MessagesResultCode::NOT_CONVERSATION_USER;
        return messagesPageResult;
    }

    // fetches messages (sorted from oldest to newest) with one extra so we know if there are older ones left
    const std::vector<Message> messages = m_messagesRepository->FindByConversationIdBefore(
        findMessagesPageByConversationIdDto.conversationId,
        findMessagesPageByConversationIdDto.cursor,
        MAX_MESSAGES_PAGE_ITEMS + 1
    );

    // Converts messages to message views
    std::vector<MessageView> messageViews = {};
    messageViews.reserve(messages.size());
    for (const Message& message : messages)
    {
        MessageView messageView = ConvertMessageToMessageView(message);
        messageViews.push_back(std::move(messageView));
    }

    const bool hasOlderMessages = messageViews.size() > MAX_MESSAGES_PAGE_ITEMS;
    if (hasOlderMessages)
    {
        /**
         * NOTES:
         *  - One extra message was asked to know if older messages are left
         *  - The extra message is the oldest one (first) and will be sent in the next page
         */
        messageViews.erase(messageViews.begin());
        messagesPageResult.data.nextCursor = messageViews.front().id;
    }

    messagesPageResult.data.items = std::move(messageViews);

    return messagesPageResult;
}

MessagesService::MessagesReadStateResult MessagesService::FindReadStateByConversationId(const FindMessagesReadStateByConversationIdDto& findMessagesReadStateByConversationIdDto)
{
    MessagesReadStateResult messagesReadStateResult = {};

    // Finds where uset stopped reading (also makes sure user is in the conversation)
    ConversationsService::FindLastReadMessagePositionByIdDto findLastReadMessagePositionByIdDto = {};
    findLastReadMessagePositionByIdDto.id = findMessagesReadStateByConversationIdDto.conversationId;
    findLastReadMessagePositionByIdDto.userId = findMessagesReadStateByConversationIdDto.userId;
    const ConversationsService::LastReadMessagePositionResult lastReadMessagePositionResult = m_conversationsService.FindLastReadMessagePositionById(findLastReadMessagePositionByIdDto);
    if (lastReadMessagePositionResult.code == ConversationsService::ConversationsResultCode::UNKNOWN_CONVERSATION)
    {
        messagesReadStateResult.code = MessagesResultCode::NOT_CONVERSATION_USER;
        return messagesReadStateResult;
    }

    const std::uint64_t lastReadMessagePosition = lastReadMessagePositionResult.data;

    // Counts unread messages after the last read message position (newest message on client's screen) and finds the newest message of the conversation (newest message on the server)
    const IMessagesRepository::MessagesCount messagesCount = m_messagesRepository->CountByConversationIdAfter(
        findMessagesReadStateByConversationIdDto.conversationId,
        findMessagesReadStateByConversationIdDto.userId,
        lastReadMessagePosition
    );

    MessagesReadStateView messagesReadStateView = {};
    messagesReadStateView.conversationId = findMessagesReadStateByConversationIdDto.conversationId;
    messagesReadStateView.lastReadMessagePosition = lastReadMessagePosition;
    messagesReadStateView.latestMessagePosition = messagesCount.latestMessagePosition;
    messagesReadStateView.unreadCount = messagesCount.count;

    messagesReadStateResult.data = std::move(messagesReadStateView);

    return messagesReadStateResult;
}

MessagesService::MessagesReadStateResult MessagesService::MarkRead(const MarkMessagesReadDto& markMessagesReadDto)
{
    MessagesReadStateResult messagesReadStateResult = {};

    // Finds newest message on client's screen (newest message) and make sure it belongs to the conversation
    const std::optional<Message> message = m_messagesRepository->FindById(markMessagesReadDto.messageId);
    if (!message.has_value() || message.value().conversationId != markMessagesReadDto.conversationId)
    {
        messagesReadStateResult.code = MessagesResultCode::UNKNOWN_MESSAGE;
        return messagesReadStateResult;
    }

    // Advances position of last read message (updates lastReadMessagePosition of conversation user if current message position is bigger)
    ConversationsService::AdvanceLastReadMessagePositionDto advanceLastReadMessagePositionDto = {};
    advanceLastReadMessagePositionDto.id = markMessagesReadDto.conversationId;
    advanceLastReadMessagePositionDto.userId = markMessagesReadDto.userId;
    advanceLastReadMessagePositionDto.messagePosition = message.value().position;
    const ConversationsService::LastReadMessagePositionResult lastReadMessagePositionResult = m_conversationsService.AdvanceLastReadMessagePosition(advanceLastReadMessagePositionDto);
    if (lastReadMessagePositionResult.code == ConversationsService::ConversationsResultCode::UNKNOWN_CONVERSATION)
    {
        messagesReadStateResult.code = MessagesResultCode::NOT_CONVERSATION_USER;
        return messagesReadStateResult;
    }

    const std::uint64_t lastReadMessagePosition = lastReadMessagePositionResult.data;

    // Counts unread messages after the last read message position (newest message on client's screen) and finds the newest message of the conversation (newest message on the server)
    const IMessagesRepository::MessagesCount messagesCount = m_messagesRepository->CountByConversationIdAfter(
        markMessagesReadDto.conversationId,
        markMessagesReadDto.userId,
        lastReadMessagePosition
    );

    MessagesReadStateView messagesReadStateView = {};
    messagesReadStateView.conversationId = markMessagesReadDto.conversationId;
    messagesReadStateView.lastReadMessagePosition = lastReadMessagePosition;
    messagesReadStateView.latestMessagePosition = messagesCount.latestMessagePosition;
    messagesReadStateView.unreadCount = messagesCount.count;

    messagesReadStateResult.data = std::move(messagesReadStateView);

    return messagesReadStateResult;
}

std::string MessagesService::ConvertMessagesResultCodeToString(MessagesResultCode messagesResultCode)
{
    switch (messagesResultCode)
    {
        case MessagesResultCode::OK:
            return "OK";

        case MessagesResultCode::NOT_CONVERSATION_USER:
            return "NOT CONVERSATION USER";

        case MessagesResultCode::TEXT_EMPTY:
            return "TEXT EMPTY";

        case MessagesResultCode::TEXT_TOO_LONG:
            return "TEXT TOO LONG";

        case MessagesResultCode::UNKNOWN_MESSAGE:
            return "UNKNOWN MESSAGE";

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return "Unknown messages result code";
    }
}

// ***********
// * PRIVATE *
// ***********
MessageView MessagesService::ConvertMessageToMessageView(const Message& message) const
{
    MessageView messageView = {};
    messageView.id = message.id;
    messageView.conversationId = message.conversationId;
    messageView.position = message.position;
    messageView.senderId = message.senderId;
    messageView.text = message.text;
    messageView.createdAt = message.createdAt;

    return messageView;
}

bool MessagesService::IsConversationUser(const std::string& conversationId, const std::string& userId) const
{
    // NOTE: Only a user of the conversation can post in it
    ConversationsService::IsConversationUserDto isConversationUserDto = {};
    isConversationUserDto.id = conversationId;
    isConversationUserDto.userId = userId;
    const bool isConversationUser = m_conversationsService.IsConversationUser(isConversationUserDto);

    return isConversationUser;
}
