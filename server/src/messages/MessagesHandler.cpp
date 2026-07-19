#include "messages/MessagesHandler.h"

#include "socket/SocketServer.h"

// **********
// * PUBLIC *
// **********
MessagesHandler::MessagesHandler(SocketServer& socketServer, MessagesService& messagesService) : m_socketServer(socketServer), m_messagesService(messagesService)
{}

SocketServerEventHandler MessagesHandler::GetCreateMessageHandler()
{
    return [this](const std::string& serializedCreateMessageSocketEventPayload, int clientSocket) {
        // Gets create message socket event payload
        const CreateMessageSocketEventPayload& createMessageSocketEventPayload = m_createMessageSocketEventPayloadDeserializer.Deserialize(serializedCreateMessageSocketEventPayload);

        // Creates message
        CreateMessageDto createMessageDto = {};
        createMessageDto.conversationId = createMessageSocketEventPayload.conversationID;
        createMessageDto.senderId = m_socketServer.GetSocketConnectionUser(clientSocket)->id;
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
