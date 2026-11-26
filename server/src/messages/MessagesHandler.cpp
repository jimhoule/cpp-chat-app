#include "messages/MessagesHandler.h"

#include "log/Logger.h"
#include "exceptions/ExpectedException.h"
#include "socket/SocketErrorCode.h"

// **********
// * PUBLIC *
// **********
MessagesHandler::MessagesHandler(SocketServer& socketServer, MessagesService& messagesService, Logger& logger)
    : m_socketServer(socketServer)
    , m_messagesService(messagesService)
    , m_logger(logger)
{}

void MessagesHandler::HandleCreateMessage(const SocketServer::EventContext& context)
{
    // Gets create message socket event payload
    const CreateMessageSocketEventPayload createMessageSocketEventPayload = m_createMessageSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

    // Creates message
    MessagesService::CreateMessageDto createMessageDto = {};
    createMessageDto.conversationId = createMessageSocketEventPayload.conversationId;
    createMessageDto.senderId = *context.userId;
    createMessageDto.text = createMessageSocketEventPayload.text;
    const MessagesService::CreateMessageResult createMessageResult = m_messagesService.Create(createMessageDto);
    if (createMessageResult.code != MessagesService::MessagesResultCode::OK)
    {
        const std::string messagesResultCodeString = m_messagesService.ConvertMessagesResultCodeToString(createMessageResult.code);
        const SocketErrorCode socketErrorCode = ConvertMessagesResultCodeToSocketErrorCode(createMessageResult.code);
        throw ExpectedException(
            socketErrorCode, 
            "Create message failed for user " + *context.userId + ", " + messagesResultCodeString
        );
    }

    const MessagesService::CreateMessageData& createMessageData = createMessageResult.data.value();

    // Serializes conversation reopened socket event
    const ConversationReopenedSocketEventPayload conversationReopenedSocketEventPayload(createMessageData.conversation);
    const ConversationReopenedSocketEvent conversationReopenedSocketEvent(conversationReopenedSocketEventPayload);
    const std::string serializedConversationReopenedSocketEvent = m_conversationReopenedSocketEventSerializer.Serialize(conversationReopenedSocketEvent);

    // Sends conversation reopened socket event (Sends before message so the client of a user who closed the conversation knows it when the message arrives)
    for (const std::string& reopenedUserId : createMessageData.reopenedUserIds)
    {
        m_socketServer.SendTo(reopenedUserId, serializedConversationReopenedSocketEvent);
    }
    
    // Serializes message created socket event
    const MessageCreatedSocketEventPayload messageCreatedSocketEventPayload(createMessageData.message);
    const MessageCreatedSocketEvent messageCreatedSocketEvent(messageCreatedSocketEventPayload);
    const std::string serializedMessageCreatedSocketEvent = m_messageCreatedSocketEventSerializer.Serialize(messageCreatedSocketEvent);

    // Sends message created socket event (Sends to all users including the sender for their other devices)
    for (const ConversationUserView& conversationUser : createMessageData.conversation.users)
    {
        m_socketServer.SendTo(conversationUser.user.id, serializedMessageCreatedSocketEvent);
    }
}

void MessagesHandler::HandleFindAllOpenMessagesReadStatesByUserId(const SocketServer::EventContext& context)
{
    // Fetches all open messages read states
    MessagesService::FindAllOpenMessagesReadStatesByUserIdDto findAllOpenMessagesReadStatesByUserIdDto = {};
    findAllOpenMessagesReadStatesByUserIdDto.userId = *context.userId;
    const MessagesService::MessagesReadStatesResult messagesReadStatesResult = m_messagesService.FindAllOpenReadStatesByUserId(findAllOpenMessagesReadStatesByUserIdDto);
    if (messagesReadStatesResult.code != MessagesService::MessagesResultCode::OK)
    {
        const std::string messagesResultCodeString = m_messagesService.ConvertMessagesResultCodeToString(messagesReadStatesResult.code);
        const SocketErrorCode socketErrorCode = ConvertMessagesResultCodeToSocketErrorCode(messagesReadStatesResult.code);
        throw ExpectedException(
            socketErrorCode,
            "Find all open messages read states failed for user " + *context.userId + ", " + messagesResultCodeString
        );
    }

    // Serializes all open messages read states found by user id socket event
    const AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload allOpenMessagesReadStatesFoundByUserIdSocketEventPayload(messagesReadStatesResult.data);
    const AllOpenMessagesReadStatesFoundByUserIdSocketEvent allOpenMessagesReadStatesFoundByUserIdSocketEvent(allOpenMessagesReadStatesFoundByUserIdSocketEventPayload);
    const std::string serializedAllOpenMessagesReadStatesFoundByUserIdSocketEvent = m_allOpenMessagesReadStatesFoundByUserIdSocketEventSerializer.Serialize(allOpenMessagesReadStatesFoundByUserIdSocketEvent);

    // Sends serialized all open messages read states found by user id socket event
    m_socketServer.SendTo(context.clientSocket, serializedAllOpenMessagesReadStatesFoundByUserIdSocketEvent);
}

void MessagesHandler::HandleFindMessagesPageByConversationId(const SocketServer::EventContext& context)
{
    // Gets find messages page by conversation id socket event payload
    const FindMessagesPageByConversationIdSocketEventPayload findMessagesPageByConversationIdSocketEventPayload = m_findMessagesPageByConversationIdSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

    // Fetches messages page
    MessagesService::FindMessagesPageByConversationIdDto findMessagesPageByConversationIdDto = {};
    findMessagesPageByConversationIdDto.conversationId = findMessagesPageByConversationIdSocketEventPayload.conversationId;
    findMessagesPageByConversationIdDto.cursor = findMessagesPageByConversationIdSocketEventPayload.cursor;
    findMessagesPageByConversationIdDto.userId = *context.userId;
    const MessagesService::MessagesPageResult messagesPageResult = m_messagesService.FindPageByConversationId(findMessagesPageByConversationIdDto);
    if (messagesPageResult.code != MessagesService::MessagesResultCode::OK)
    {
        const std::string messagesResultCodeString = m_messagesService.ConvertMessagesResultCodeToString(messagesPageResult.code);
        const SocketErrorCode socketErrorCode = ConvertMessagesResultCodeToSocketErrorCode(messagesPageResult.code);
        throw ExpectedException(
            socketErrorCode, 
            "Find messages page failed for user " + *context.userId + ", " + messagesResultCodeString
        );
    }

    // Serializes messages page found by conversation id socket event
    const MessagesPageFoundByConversationIdSocketEventPayload messagesPageFoundByConversationIdSocketEventPayload(
        messagesPageResult.data,
        findMessagesPageByConversationIdSocketEventPayload.conversationId
    );
    const MessagesPageFoundByConversationIdSocketEvent messagesPageFoundByConversationIdSocketEvent(messagesPageFoundByConversationIdSocketEventPayload);
    const std::string serializedMessagesPageFoundByConversationIdSocketEvent = m_messagesPageFoundByConversationIdSocketEventSerializer.Serialize(messagesPageFoundByConversationIdSocketEvent);

    // Sends messages page found by conversation id socket event
    m_socketServer.SendTo(context.clientSocket, serializedMessagesPageFoundByConversationIdSocketEvent);
}

void MessagesHandler::HandleFindMessagesReadStateByConversationId(const SocketServer::EventContext& context)
{
    // Gets find messages read state by conversation id socket event payload
    const FindMessagesReadStateByConversationIdSocketEventPayload findMessagesReadStateByConversationIdSocketEventPayload = m_findMessagesReadStateByConversationIdSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

    // Fetches messages read state 
    MessagesService::FindMessagesReadStateByConversationIdDto findMessagesReadStateByConversationIdDto = {};
    findMessagesReadStateByConversationIdDto.conversationId = findMessagesReadStateByConversationIdSocketEventPayload.conversationId;
    findMessagesReadStateByConversationIdDto.userId = *context.userId;
    const MessagesService::MessagesReadStateResult messagesReadStateResult = m_messagesService.FindReadStateByConversationId(findMessagesReadStateByConversationIdDto);
    if (messagesReadStateResult.code != MessagesService::MessagesResultCode::OK)
    {
        const std::string messagesResultCodeString = m_messagesService.ConvertMessagesResultCodeToString(messagesReadStateResult.code);
        const SocketErrorCode socketErrorCode = ConvertMessagesResultCodeToSocketErrorCode(messagesReadStateResult.code);
        throw ExpectedException(
            socketErrorCode, 
            "Find messages read state failed for user " + *context.userId + ", " + messagesResultCodeString
        );
    }

    // Serializes messages read state found by conversation id socket event
    const MessagesReadStateFoundByConversationIdSocketEventPayload messagesReadStateFoundByConversationIdSocketEventPayload(messagesReadStateResult.data.value());
    const MessagesReadStateFoundByConversationIdSocketEvent messagesReadStateFoundByConversationIdSocketEvent(messagesReadStateFoundByConversationIdSocketEventPayload);
    const std::string serializedMessagesReadStateFoundByConversationIdSocketEvent = m_messagesReadStateFoundByConversationIdSocketEventSerializer.Serialize(messagesReadStateFoundByConversationIdSocketEvent);

    // Sends messages read state found by conversation id socket event
    m_socketServer.SendTo(context.clientSocket, serializedMessagesReadStateFoundByConversationIdSocketEvent);
}

void MessagesHandler::HandleMarkMessagesRead(const SocketServer::EventContext& context)
{
    // Gets mark messages read socket event payload
    const MarkMessagesReadSocketEventPayload markMessagesReadSocketEventPayload = m_markMessagesReadSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

    // Marks messages as read
    MessagesService::MarkMessagesReadDto markMessagesReadDto = {};
    markMessagesReadDto.conversationId = markMessagesReadSocketEventPayload.conversationId;
    markMessagesReadDto.messageId = markMessagesReadSocketEventPayload.messageId;
    markMessagesReadDto.userId = *context.userId;
    const MessagesService::MessagesReadStateResult messagesReadStateResult = m_messagesService.MarkRead(markMessagesReadDto);
    if (messagesReadStateResult.code != MessagesService::MessagesResultCode::OK)
    {
        const std::string messagesResultCodeString = m_messagesService.ConvertMessagesResultCodeToString(messagesReadStateResult.code);
        const SocketErrorCode socketErrorCode = ConvertMessagesResultCodeToSocketErrorCode(messagesReadStateResult.code);
        throw ExpectedException(
            socketErrorCode, 
            "Mark messages read failed for user " + *context.userId + ", " + messagesResultCodeString
        );
    }

    // Serializes messages marked read socket event
    const MessagesMarkedReadSocketEventPayload messagesMarkedReadSocketEventPayload(messagesReadStateResult.data.value());
    const MessagesMarkedReadSocketEvent messagesMarkedReadSocketEvent(messagesMarkedReadSocketEventPayload);
    const std::string serializedMessagesMarkedReadSocketEvent = m_messagesMarkedReadSocketEventSerializer.Serialize(messagesMarkedReadSocketEvent);

    /**
     * NOTES:
     *  - Sends messages marked read socket event 
     *  - Sends to all sockets of the user so their devices update their badge too
     */
    m_socketServer.SendTo(*context.userId, serializedMessagesMarkedReadSocketEvent);
}

// ***********
// * PRIVATE *
// ***********
SocketErrorCode MessagesHandler::ConvertMessagesResultCodeToSocketErrorCode(MessagesService::MessagesResultCode messagesResultCode)
{
    switch (messagesResultCode)
    {
        case MessagesService::MessagesResultCode::TEXT_EMPTY:
        case MessagesService::MessagesResultCode::TEXT_TOO_LONG:
            return SocketErrorCode::INVALID_PAYLOAD;

        // NOTE: NOT_CONVERSATION_USER and UNKNOWN_MESSAGE look just like an unknown conversation to a client
        case MessagesService::MessagesResultCode::NOT_CONVERSATION_USER:
        case MessagesService::MessagesResultCode::UNKNOWN_MESSAGE:
            return SocketErrorCode::NOT_FOUND;

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return SocketErrorCode::INTERNAL;
    }
}
