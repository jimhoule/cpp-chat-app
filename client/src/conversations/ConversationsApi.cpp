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

    m_socketClient.On(SocketEventName::ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID, this, HandleAllOpenConversationsFound);
}

ConversationsApi::~ConversationsApi()
{
    m_socketClient.Off(SocketEventName::ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID, this);
}

IObservable<AllOpenConversationsFoundEvent>& ConversationsApi::GetAllOpenConversationsFoundSubject()
{
    return m_allOpenConversationsFoundSubject;
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
