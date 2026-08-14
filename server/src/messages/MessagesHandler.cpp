#include "messages/MessagesHandler.h"

#include "log/Logger.h"
#include "messages/MessagesService.h"
#include "exceptions/ExpectedException.h"
#include "socket/SocketErrorCode.h"
#include "socket/SocketEventContext.h"
#include "socket/SocketServer.h"

// **********
// * PUBLIC *
// **********
MessagesHandler::MessagesHandler(SocketServer& socketServer, MessagesService& messagesService, Logger& logger)
    : m_socketServer(socketServer)
    , m_messagesService(messagesService)
    , m_logger(logger)
{}

SocketServerEventHandler MessagesHandler::GetCreateMessageHandler()
{
    return [this](const SocketEventContext& context) {
        // Gets create message socket event payload
        const CreateMessageSocketEventPayload createMessageSocketEventPayload = m_createMessageSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Creates message
        CreateMessageDto createMessageDto = {};
        createMessageDto.conversationId = createMessageSocketEventPayload.conversationId;
        createMessageDto.senderId = context.user->id;
        createMessageDto.text = createMessageSocketEventPayload.text;
        const MessageResult messageResult = m_messagesService.Create(createMessageDto);
        if (messageResult.code != MessagesResultCode::OK)
        {
            throw ExpectedException(SocketErrorCode::INVALID_PAYLOAD, "Create message failed for user " + context.user->id + ", " + ConvertMessagesResultCodeToString(messageResult.code));
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
