#pragma once

#include "deserializer/AllOpenMessagesReadStatesFoundByUserIdSocketEventPayloadDeserializer.h"
#include "deserializer/MessageCreatedSocketEventPayloadDeserializer.h"
#include "deserializer/MessagesMarkedReadSocketEventPayloadDeserializer.h"
#include "deserializer/MessagesPageFoundByConversationIdSocketEventPayloadDeserializer.h"
#include "deserializer/MessagesReadStateFoundByConversationIdSocketEventPayloadDeserializer.h"
#include "messages/AllOpenMessagesReadStatesFoundEvent.h"
#include "messages/MessageCreatedEvent.h"
#include "messages/MessagesMarkedReadEvent.h"
#include "messages/MessagesPageFoundEvent.h"
#include "messages/MessagesReadStateFoundEvent.h"
#include "observer/IObservable.h"
#include "observer/Subject.h"
#include "serializer/CreateMessageSocketEventSerializer.h"
#include "serializer/FindAllOpenMessagesReadStatesByUserIdSocketEventSerializer.h"
#include "serializer/FindMessagesPageByConversationIdSocketEventSerializer.h"
#include "serializer/FindMessagesReadStateByConversationIdSocketEventSerializer.h"
#include "serializer/MarkMessagesReadSocketEventSerializer.h"

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

    struct FindMessagesReadStateParams
    {
        std::string conversationId;
    };

    struct MarkMessagesReadParams
    {
        std::string conversationId;
        // NOTE: The newest message id on the screen (every message up to this one is marked read)
        std::string messageId;
    };

    MessagesApi(SocketClient& socketClient, Logger& logger);
    ~MessagesApi();

    // NOTE: Exposes observable view only so callers cannot notify
    IObservable<AllOpenMessagesReadStatesFoundEvent>& GetAllOpenMessagesReadStatesFoundSubject();
    IObservable<MessageCreatedEvent>& GetMessageCreatedSubject();
    IObservable<MessagesMarkedReadEvent>& GetMessagesMarkedReadSubject();
    IObservable<MessagesPageFoundEvent>& GetMessagesPageFoundSubject();
    IObservable<MessagesReadStateFoundEvent>& GetMessagesReadStateFoundSubject();

    void Create(const CreateMessageParams& createMessageParams);
    void FindAllOpenReadStates();
    void FindPage(const FindMessagesPageParams& findMessagesPageParams);
    void FindReadState(const FindMessagesReadStateParams& findMessagesReadStateParams);
    void MarkRead(const MarkMessagesReadParams& markMessagesReadParams);
    
private:
    SocketClient& m_socketClient;

    Subject<AllOpenMessagesReadStatesFoundEvent> m_allOpenMessagesReadStatesFoundSubject = {};
    Subject<MessageCreatedEvent> m_messageCreatedSubject = {};
    Subject<MessagesMarkedReadEvent> m_messagesMarkedReadSubject = {};
    Subject<MessagesPageFoundEvent> m_messagesPageFoundSubject = {};
    Subject<MessagesReadStateFoundEvent> m_messagesReadStateFoundSubject = {};

    CreateMessageSocketEventSerializer m_createMessageSocketEventSerializer = {};
    MessageCreatedSocketEventPayloadDeserializer m_messageCreatedSocketEventPayloadDeserializer = {};

    FindAllOpenMessagesReadStatesByUserIdSocketEventSerializer m_findAllOpenMessagesReadStatesByUserIdSocketEventSerializer = {};
    AllOpenMessagesReadStatesFoundByUserIdSocketEventPayloadDeserializer m_allOpenMessagesReadStatesFoundByUserIdSocketEventPayloadDeserializer = {};

    FindMessagesPageByConversationIdSocketEventSerializer m_findMessagesPageByConversationIdSocketEventSerializer = {};
    MessagesPageFoundByConversationIdSocketEventPayloadDeserializer m_messagesPageFoundByConversationIdSocketEventPayloadDeserializer = {};

    FindMessagesReadStateByConversationIdSocketEventSerializer m_findMessagesReadStateByConversationIdSocketEventSerializer = {};
    MessagesReadStateFoundByConversationIdSocketEventPayloadDeserializer m_messagesReadStateFoundByConversationIdSocketEventPayloadDeserializer = {};

    MarkMessagesReadSocketEventSerializer m_markMessagesReadSocketEventSerializer = {};
    MessagesMarkedReadSocketEventPayloadDeserializer m_messagesMarkedReadSocketEventPayloadDeserializer = {};

    Logger& m_logger;
};
