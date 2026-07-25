#include "messages/MessagesHandler.h"

#include "log/Logger.h"
#include "messages/MessagesService.h"
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
    return [this](const std::string& serializedCreateMessageSocketEventPayload, int clientSocket) {
        // Gets socket connection user
        const User* socketConnectionUser = m_socketServer.GetSocketConnectionUser(clientSocket);
        if (socketConnectionUser == nullptr)
        {
            m_logger.Warning("Cannot create message, client socket " + std::to_string(clientSocket) + " is not authenticated");
            return;
        }

        // Gets create message socket event payload
        const CreateMessageSocketEventPayload& createMessageSocketEventPayload = m_createMessageSocketEventPayloadDeserializer.Deserialize(serializedCreateMessageSocketEventPayload);

        // Creates message
        CreateMessageDto createMessageDto = {};
        createMessageDto.conversationId = createMessageSocketEventPayload.conversationID;
        createMessageDto.senderId = socketConnectionUser->id;
        createMessageDto.text = createMessageSocketEventPayload.text;
        const Message message = m_messagesService.Create(createMessageDto);
        
        // Serializes message created socket event
        const MessageCreatedSocketEventPayload& messageCreatedSocketEventPayload(message);
        const MessageCreatedSocketEvent& messageCreatedSocketEvent(messageCreatedSocketEventPayload);
        const std::string& serializedMessageCreatedSocketEvent = m_messageCreatedSocketEventSerializer.Serialize(messageCreatedSocketEvent);

        // Sends message created socket event
        // TODO: send to all users of conversation
        m_socketServer.SendTo(clientSocket, serializedMessageCreatedSocketEvent);
    };
}
