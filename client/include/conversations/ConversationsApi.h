#pragma once

#include "conversations/AllOpenConversationsFoundEvent.h"
#include "conversations/ConversationClosedEvent.h"
#include "conversations/ConversationOpenedEvent.h"
#include "conversations/ConversationReopenedEvent.h"
#include "deserializer/AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer.h"
#include "deserializer/ConversationClosedSocketEventPayloadDeserializer.h"
#include "deserializer/ConversationOpenedSocketEventPayloadDeserializer.h"
#include "deserializer/ConversationReopenedSocketEventPayloadDeserializer.h"
#include "observer/IObservable.h"
#include "observer/Subject.h"
#include "serializer/CloseConversationSocketEventSerializer.h"
#include "serializer/FindAllOpenConversationsByUserIdSocketEventSerializer.h"
#include "serializer/OpenConversationSocketEventSerializer.h"

#include <string>
#include <vector>

// Forward declarations
class Logger;
class SocketClient;

class ConversationsApi
{
public:
    struct CloseConversationParams
    {
        std::string conversationId;
    };

    struct OpenConversationParams
    {
        std::vector<std::string> userIds;
    };

    ConversationsApi(SocketClient& socketClient, Logger& logger);
    ~ConversationsApi();

    // NOTE: Exposes observable view only so callers cannot notify
    IObservable<AllOpenConversationsFoundEvent>& GetAllOpenConversationsFoundSubject();
    IObservable<ConversationClosedEvent>& GetConversationClosedSubject();
    IObservable<ConversationOpenedEvent>& GetConversationOpenedSubject();
    IObservable<ConversationReopenedEvent>& GetConversationReopenedSubject();

    void FindAllOpen();
    void Close(const CloseConversationParams& closeConversationParams);
    void Open(const OpenConversationParams& openConversationParams);
    
private:
    SocketClient& m_socketClient;

    Subject<AllOpenConversationsFoundEvent> m_allOpenConversationsFoundSubject = {};
    Subject<ConversationClosedEvent> m_conversationClosedSubject = {};
    Subject<ConversationOpenedEvent> m_conversationOpenedSubject = {};
    Subject<ConversationReopenedEvent> m_conversationReopenedSubject = {};

    FindAllOpenConversationsByUserIdSocketEventSerializer m_findAllOpenConversationsByUserIdSocketEventSerializer = {};
    AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer m_allOpenConversationsFoundByUserIdSocketEventPayloadDeserializer = {};

    CloseConversationSocketEventSerializer m_closeConversationSocketEventSerializer = {};
    ConversationClosedSocketEventPayloadDeserializer m_conversationClosedSocketEventPayloadDeserializer = {};

    OpenConversationSocketEventSerializer m_openConversationSocketEventSerializer = {};
    ConversationOpenedSocketEventPayloadDeserializer m_conversationOpenedSocketEventPayloadDeserializer = {};

    ConversationReopenedSocketEventPayloadDeserializer m_conversationReopenedSocketEventPayloadDeserializer = {};

    Logger& m_logger;
};
