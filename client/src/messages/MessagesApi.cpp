#include "messages/MessagesApi.h"

#include "socket/SocketClient.h"

// **********
// * PUBLIC *
// **********
MessagesApi::MessagesApi(SocketClient& socketClient, Logger& logger)
    : m_socketClient(socketClient)
    , m_logger(logger)
{
    SocketClient::EventHandler HandleMessageCreated = [this](const SocketClient::EventContext& context) {
        const MessageCreatedSocketEventPayload messageCreatedSocketEventPayload = m_messageCreatedSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Sends event to subcriptions
        const MessageCreatedEvent messageCreatedEvent(messageCreatedSocketEventPayload.message);
        m_messageCreatedSubject.Notify(messageCreatedEvent);
    };

    SocketClient::EventHandler HandleMessagesPageFound = [this](const SocketClient::EventContext& context) {
        const MessagesPageFoundByConversationIdSocketEventPayload messagesPageFoundByConversationIdSocketEventPayload = m_messagesPageFoundByConversationIdSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Sends event to subcriptions
        const MessagesPageFoundEvent messagesPageFoundEvent(
            messagesPageFoundByConversationIdSocketEventPayload.messagesPage,
            messagesPageFoundByConversationIdSocketEventPayload.conversationId
        );
        m_messagesPageFoundSubject.Notify(messagesPageFoundEvent);
    };

    m_socketClient.On(SocketEventName::MESSAGE_CREATED, this, HandleMessageCreated);
    m_socketClient.On(SocketEventName::MESSAGES_PAGE_FOUND_BY_CONVERSATION_ID, this, HandleMessagesPageFound);
}

MessagesApi::~MessagesApi()
{
    m_socketClient.Off(SocketEventName::MESSAGE_CREATED, this);
    m_socketClient.Off(SocketEventName::MESSAGES_PAGE_FOUND_BY_CONVERSATION_ID, this);
}

IObservable<MessageCreatedEvent>& MessagesApi::GetMessageCreatedSubject()
{
    return m_messageCreatedSubject;
}

IObservable<MessagesPageFoundEvent>& MessagesApi::GetMessagesPageFoundSubject()
{
    return m_messagesPageFoundSubject;
}

void MessagesApi::Create(const CreateMessageParams& createMessageParams)
{
    // Serializes create message socket event
    const CreateMessageSocketEventPayload createMessageSocketEventPayload(
        createMessageParams.conversationId,
        createMessageParams.text
    );
    const CreateMessageSocketEvent createMessageSocketEvent(createMessageSocketEventPayload);
    const std::string serializedCreateMessageSocketEvent = m_createMessageSocketEventSerializer.Serialize(createMessageSocketEvent);

    // Sends create message socket event
    m_socketClient.Send(serializedCreateMessageSocketEvent);
}

void MessagesApi::FindPage(const FindMessagesPageParams& findMessagesPageParams)
{
    // Serializes find messages page by conversation id socket event
    const FindMessagesPageByConversationIdSocketEventPayload findMessagesPageByConversationIdSocketEventPayload(
        findMessagesPageParams.conversationId,
        findMessagesPageParams.cursor
    );
    const FindMessagesPageByConversationIdSocketEvent findMessagesPageByConversationIdSocketEvent(findMessagesPageByConversationIdSocketEventPayload);
    const std::string serializedFindMessagesPageByConversationIdSocketEvent = m_findMessagesPageByConversationIdSocketEventSerializer.Serialize(findMessagesPageByConversationIdSocketEvent);

    // Sends find messages page by conversation ide socket event
    m_socketClient.Send(serializedFindMessagesPageByConversationIdSocketEvent);
}
