#include "messages/MessagesService.h"

#include "conversations/ConversationsService.h"
#include "log/Logger.h"
#include "uuid/UuidService.h"
#include "views/ConversationUserView.h"

constexpr size_t MAX_MESSAGE_TEXT_LENGTH = 4000;

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
    ConversationsService::IsConversationUserDto isConversationUserDto = {};
    isConversationUserDto.conversationId = createMessageDto.conversationId;
    isConversationUserDto.userId = createMessageDto.senderId;
    const bool isConversationUser = m_conversationsService.IsConversationUser(isConversationUserDto);
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

    const MessageView messageView = ConvertMessageToMessageView(m_messagesRepository->Create(message));

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
    createMessageData.message = messageView;

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
    openConversationForAllUsersDto.conversationId = messageView.conversationId;
    const ConversationsService::ConversationResult openedConversationResult = m_conversationsService.OpenForAllUsers(openConversationForAllUsersDto);

    createMessageData.conversation = openedConversationResult.data.value();
    createMessageResult.data = createMessageData;

    return createMessageResult;
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
    messageView.senderId = message.senderId;
    messageView.text = message.text;
    messageView.createdAt = message.createdAt;

    return messageView;
}

