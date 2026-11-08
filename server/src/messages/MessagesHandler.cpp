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

SocketServer::EventHandler MessagesHandler::GetFindMessagesPageByConversationIdHandler()
{
    return [this](const SocketServer::EventContext& context) {
        // Gets find messages page by coonversation id socket event payload
        const FindMessagesPageByConversationIdSocketEventPayload findMessagesPageByConversationIdSocketEventPayload = m_findMessagesPageByConversationIdSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Fetches messages page
        MessagesService::FindMessagesPageByConversationIdDto findMessagesPageByConversationIdDto = {};
        findMessagesPageByConversationIdDto.conversationId = findMessagesPageByConversationIdSocketEventPayload.conversationId;
        findMessagesPageByConversationIdDto.cursor = findMessagesPageByConversationIdSocketEventPayload.cursor;
        findMessagesPageByConversationIdDto.userId = context.user->id;

        const MessagesService::MessagesPageResult messagesPageResult = m_messagesService.FindPageByConversationId(findMessagesPageByConversationIdDto);
        if (messagesPageResult.code != MessagesService::MessagesResultCode::OK)
        {
            const std::string messagesResultCodeString = m_messagesService.ConvertMessagesResultCodeToString(messagesPageResult.code);
            const SocketErrorCode socketErrorCode = ConvertMessagesResultCodeToSocketErrorCode(messagesPageResult.code);
            throw ExpectedException(
                socketErrorCode, 
                "Fetch messages page failed for user " + context.user->id + ", " + messagesResultCodeString
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
    };
}

SocketServer::EventHandler MessagesHandler::GetCreateMessageHandler()
{
    return [this](const SocketServer::EventContext& context) {
        // Gets create message socket event payload
        const CreateMessageSocketEventPayload createMessageSocketEventPayload = m_createMessageSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Creates message
        MessagesService::CreateMessageDto createMessageDto = {};
        createMessageDto.conversationId = createMessageSocketEventPayload.conversationId;
        createMessageDto.senderId = context.user->id;
        createMessageDto.text = createMessageSocketEventPayload.text;
        const MessagesService::CreateMessageResult createMessageResult = m_messagesService.Create(createMessageDto);
        if (createMessageResult.code != MessagesService::MessagesResultCode::OK)
        {
            const std::string messagesResultCodeString = m_messagesService.ConvertMessagesResultCodeToString(createMessageResult.code);
            const SocketErrorCode socketErrorCode = ConvertMessagesResultCodeToSocketErrorCode(createMessageResult.code);
            throw ExpectedException(
                socketErrorCode, 
                "Create message failed for user " + context.user->id + ", " + messagesResultCodeString
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
    };
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

            // NOTE: NOT_CONVERSATION_USER looks just like an unknown conversation to a client
        case MessagesService::MessagesResultCode::NOT_CONVERSATION_USER:
            return SocketErrorCode::NOT_FOUND;

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return SocketErrorCode::INTERNAL;
    }
}
