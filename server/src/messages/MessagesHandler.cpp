#include "messages/MessagesHandler.h"

// **********
// * PUBLIC *
// **********
MessagesHandler::MessagesHandler(SocketServer& socketServer, const MessagesService& messagesService) : m_socketServer(socketServer), m_messagesService(messagesService)
{}

SocketServerEventHandler MessagesHandler::GetCreateMessageHandler()
{
    return [this](const std::string& serializedCreateMessageSocketEventPayload, int clientSocket) {
        // Gets create message socket event payload
        const CreateMessageSocketEventPayload& createMessageSocketEventPayload = m_createMessageSocketEventPayloadDeserializer.Deserialize(serializedCreateMessageSocketEventPayload);

        // Creates message
        // TODO: Create message service
        Message fakeMessage = {};
        fakeMessage.id = "FakeId";
        fakeMessage.conversationId = "fakeConversationId";
        fakeMessage.senderId = "fakeSenderId";
        fakeMessage.text = "fakeText";
        fakeMessage.createdAt = std::time(0);
        
        // Serializes message created socket event
        const MessageCreatedSocketEventPayload& messageCreatedSocketEventPayload(fakeMessage);
        const MessageCreatedSocketEvent& messageCreatedSocketEvent(messageCreatedSocketEventPayload);
        const std::string& serializedMessageCreatedSocketEvent = m_messageCreatedSocketEventSerializer.Serialize(messageCreatedSocketEvent);

        // Sends message created socket event
        // TODO: send to all users of conversation
        m_socketServer.SendTo(clientSocket, serializedMessageCreatedSocketEvent);
    };
}
