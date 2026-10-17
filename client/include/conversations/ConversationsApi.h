#pragma once

#include "conversations/AllOpenConversationsFoundEvent.h"
#include "deserializer/AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer.h"
#include "observer/IObservable.h"
#include "observer/Subject.h"
#include "serializer/FindAllOpenConversationsByUserIdSocketEventSerializer.h"

// Forward declarations
class Logger;
class SocketClient;

class ConversationsApi
{
public:
    ConversationsApi(SocketClient& socketClient, Logger& logger);
    ~ConversationsApi();

    // NOTE: Exposes observable view only so callers cannot notify
    IObservable<AllOpenConversationsFoundEvent>& GetAllOpenConversationsFoundSubject();

    void FindAllOpen();
    
private:
    SocketClient& m_socketClient;

    Subject<AllOpenConversationsFoundEvent> m_allOpenConversationsFoundSubject = {};

    FindAllOpenConversationsByUserIdSocketEventSerializer m_findAllOpenConversationsByUserIdSocketEventSerializer = {};
    AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer m_allOpenConversationsFoundByUserIdSocketEventPayloadDeserializer = {};

    Logger& m_logger;
};
