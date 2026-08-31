#include "messages/MessagesApi.h"

#include "socket/SocketClient.h"

// **********
// * PUBLIC *
// **********
MessagesApi::MessagesApi(SocketClient& socketClient, Logger& logger)
    : m_socketClient(socketClient)
    , m_logger(logger)
{
    SocketClient::EventHandler HandleMessageCreated = [this](const std::string& serializedMessageCreatedSocketEventPayload) {
        const MessageCreatedSocketEventPayload messageCreatedSocketEventPayload = m_messageCreatedSocketEventPayloadDeserializer.Deserialize(serializedMessageCreatedSocketEventPayload);

        // Sends event to subcriptions
        const MessageCreatedEvent messageCreatedEvent(messageCreatedSocketEventPayload.message);
        m_messageCreatedSubject.Notify(messageCreatedEvent);
    };

    m_socketClient.On(SocketEventName::MESSAGE_CREATED, HandleMessageCreated);
}

MessagesApi::~MessagesApi()
{
    m_socketClient.Off(SocketEventName::MESSAGE_CREATED);
}

IObservable<MessageCreatedEvent>& MessagesApi::GetMessageCreatedSubject()
{
    return m_messageCreatedSubject;
}

void MessagesApi::Create(const CreateMessageParams& createMessageParams)
{
    // Serializes create message socket event
    const CreateMessageSocketEventPayload createMessageSocketEventPayload(createMessageParams.conversationId, createMessageParams.text);
    const CreateMessageSocketEvent createMessageSocketEvent(createMessageSocketEventPayload);
    const std::string serializedCreateMessageSocketEvent = m_createMessageSocketEventSerializer.Serialize(createMessageSocketEvent);

    // Sends create message socket event
    m_socketClient.Send(serializedCreateMessageSocketEvent);
}
