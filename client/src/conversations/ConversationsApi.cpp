#include "conversations/ConversationsApi.h"

#include "socket/SocketClient.h"

// **********
// * PUBLIC *
// **********
ConversationsApi::ConversationsApi(SocketClient& socketClient, Logger& logger)
    : m_socketClient(socketClient)
    , m_logger(logger)
{
    SocketClient::EventHandler HandleAllOpenConversationsFound = [this](const std::string& serializedAllOpenConversationsFoundByUserIdSocketEventPayload) {
        const AllOpenConversationsFoundByUserIdSocketEventPayload allOpenConversationsFoundByUserIdSocketEventPayload = m_allOpenConversationsFoundByUserIdSocketEventPayloadDeserializer.Deserialize(serializedAllOpenConversationsFoundByUserIdSocketEventPayload);

        // Sends event to subcriptions
        const AllOpenConversationsFoundEvent allOpenConversationsFoundEvent(allOpenConversationsFoundByUserIdSocketEventPayload.conversations);
        m_allOpenConversationsFoundSubject.Notify(allOpenConversationsFoundEvent);
    };

    SocketClient::EventHandler HandleConversationClosed = [this](const std::string& serializedConversationClosedSocketEventPayload) {
        const ConversationClosedSocketEventPayload conversationClosedEventPayload = m_conversationClosedSocketEventPayloadDeserializer.Deserialize(serializedConversationClosedSocketEventPayload);

        // Sends event to subcriptions
        const ConversationClosedEvent conversationClosedEvent(conversationClosedEventPayload.conversation);
        m_conversationClosedSubject.Notify(conversationClosedEvent);
    };

    SocketClient::EventHandler HandleConversationOpened = [this](const std::string& serializedConversationOpenedSocketEventPayload) {
        const ConversationOpenedSocketEventPayload conversationOpenedSocketEventPayload = m_conversationOpenedSocketEventPayloadDeserializer.Deserialize(serializedConversationOpenedSocketEventPayload);

        // Sends event to subcriptions
        const ConversationOpenedEvent conversationOpenedEvent(conversationOpenedSocketEventPayload.conversation);
        m_conversationOpenedSubject.Notify(conversationOpenedEvent);
    };

    m_socketClient.On(SocketEventName::ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID, this, HandleAllOpenConversationsFound);
    m_socketClient.On(SocketEventName::CONVERSATION_CLOSED, this, HandleConversationClosed);
    m_socketClient.On(SocketEventName::CONVERSATION_OPENED, this, HandleConversationOpened);
}

ConversationsApi::~ConversationsApi()
{
    m_socketClient.Off(SocketEventName::ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID, this);
    m_socketClient.Off(SocketEventName::CONVERSATION_CLOSED, this);
    m_socketClient.Off(SocketEventName::CONVERSATION_OPENED, this);
}

IObservable<AllOpenConversationsFoundEvent>& ConversationsApi::GetAllOpenConversationsFoundSubject()
{
    return m_allOpenConversationsFoundSubject;
}

IObservable<ConversationClosedEvent>& ConversationsApi::GetConversationClosedSubject()
{
    return m_conversationClosedSubject;
}

IObservable<ConversationOpenedEvent>& ConversationsApi::GetConversationOpenedSubject()
{
    return m_conversationOpenedSubject;
}

void ConversationsApi::FindAllOpen()
{
    // Serializes find all open conversations by user id socket event
    const FindAllOpenConversationsByUserIdSocketEventPayload findAllOpenConversationsByUserIdSocketEventPayload;
    const FindAllOpenConversationsByUserIdSocketEvent findAllOpenConversationsByUserIdSocketEvent(findAllOpenConversationsByUserIdSocketEventPayload);
    const std::string serializedFindAllOpenConversationsByUserIdSocketEvent = m_findAllOpenConversationsByUserIdSocketEventSerializer.Serialize(findAllOpenConversationsByUserIdSocketEvent);

    // Sends find all open conversations by user id socket event
    m_socketClient.Send(serializedFindAllOpenConversationsByUserIdSocketEvent);
}

void ConversationsApi::Close(const CloseConversationParams& closeConversationParams)
{
    // Serializes close conversation socket event
    const CloseConversationSocketEventPayload closeConversationSocketEventPayload(closeConversationParams.conversationId);
    const CloseConversationSocketEvent closeConversationSocketEvent(closeConversationSocketEventPayload);
    const std::string serializedCloseConversationSocketEvent = m_closeConversationSocketEventSerializer.Serialize(closeConversationSocketEvent);

    // Sends close conversation socket event
    m_socketClient.Send(serializedCloseConversationSocketEvent);
}

void ConversationsApi::Open(const OpenConversationParams& openConversationParams)
{
    // Serializes open conversation socket event
    const OpenConversationSocketEventPayload openConversationSocketEventPayload(openConversationParams.userIds);
    const OpenConversationSocketEvent openConversationSocketEvent(openConversationSocketEventPayload);
    const std::string serializedOpenConversationSocketEvent = m_openConversationSocketEventSerializer.Serialize(openConversationSocketEvent);

    // Sends open conversation socket event
    m_socketClient.Send(serializedOpenConversationSocketEvent);
}
