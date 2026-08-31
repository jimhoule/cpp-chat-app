#include "messages/MessagesHandler.h"

#include "log/Logger.h"
#include "messages/MessagesService.h"
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
        const MessagesService::MessageResult messageResult = m_messagesService.Create(createMessageDto);
        if (messageResult.code != MessagesService::MessagesResultCode::OK)
        {
            const std::string messagesResultCodeString = m_messagesService.ConvertMessagesResultCodeToString(messageResult.code);
            throw ExpectedException(SocketErrorCode::INVALID_PAYLOAD, "Create message failed for user " + context.user->id + ", " + messagesResultCodeString);
        }
        
        // Serializes message created socket event
        const MessageCreatedSocketEventPayload messageCreatedSocketEventPayload(messageResult.data.value());
        const MessageCreatedSocketEvent messageCreatedSocketEvent(messageCreatedSocketEventPayload);
        const std::string serializedMessageCreatedSocketEvent = m_messageCreatedSocketEventSerializer.Serialize(messageCreatedSocketEvent);

        // Sends message created socket event
        // TODO: send to all users of conversation
        m_socketServer.SendTo(context.clientSocket, serializedMessageCreatedSocketEvent);
    };
}
