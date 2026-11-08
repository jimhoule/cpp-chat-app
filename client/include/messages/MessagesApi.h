#pragma once

#include "deserializer/MessageCreatedSocketEventPayloadDeserializer.h"
#include "deserializer/MessagesPageFoundByConversationIdSocketEventPayloadDeserializer.h"
#include "messages/MessageCreatedEvent.h"
#include "messages/MessagesPageFoundEvent.h"
#include "observer/IObservable.h"
#include "observer/Subject.h"
#include "serializer/CreateMessageSocketEventSerializer.h"
#include "serializer/FindMessagesPageByConversationIdSocketEventSerializer.h"

#include <string>

// Forward declarations
class Logger;
class SocketClient;

class MessagesApi
{
public:
    struct CreateMessageParams
    {
        std::string conversationId;
        std::string text;
    };

    struct FindMessagesPageParams
    {
        std::string conversationId;
        std::string cursor;
    };

    MessagesApi(SocketClient& socketClient, Logger& logger);
    ~MessagesApi();

    // NOTE: Exposes observable view only so callers cannot notify
    IObservable<MessageCreatedEvent>& GetMessageCreatedSubject();
    IObservable<MessagesPageFoundEvent>& GetMessagesPageFoundSubject();

    void Create(const CreateMessageParams& createMessageParams);
    void FindPage(const FindMessagesPageParams& findMessagesPageParams);
    
private:
    SocketClient& m_socketClient;

    Subject<MessageCreatedEvent> m_messageCreatedSubject = {};
    Subject<MessagesPageFoundEvent> m_messagesPageFoundSubject = {};

    CreateMessageSocketEventSerializer m_createMessageSocketEventSerializer = {};
    MessageCreatedSocketEventPayloadDeserializer m_messageCreatedSocketEventPayloadDeserializer = {};

    FindMessagesPageByConversationIdSocketEventSerializer m_findMessagesPageByConversationIdSocketEventSerializer = {};
    MessagesPageFoundByConversationIdSocketEventPayloadDeserializer m_messagesPageFoundByConversationIdSocketEventPayloadDeserializer = {};

    Logger& m_logger;
};
