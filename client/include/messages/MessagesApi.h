#pragma once

#include "deserializer/MessageCreatedSocketEventPayloadDeserializer.h"
#include "messages/MessageCreatedEvent.h"
#include "observer/IObservable.h"
#include "observer/Subject.h"
#include "serializer/CreateMessageSocketEventSerializer.h"

// Forward declarations
class Logger;
class SocketClient;

struct CreateMessageParams
{
    std::string conversationId;
    std::string text;
};

class MessagesApi
{
public:
    MessagesApi(SocketClient& socketClient, Logger& logger);
    ~MessagesApi();

    // NOTE: Exposes observable view only so callers cannot notify
    IObservable<MessageCreatedEvent>& GetMessageCreatedSubject();

    void Create(const CreateMessageParams& createMessageParams);
    
private:
    SocketClient& m_socketClient;

    Subject<MessageCreatedEvent> m_messageCreatedSubject = {};

    CreateMessageSocketEventSerializer m_createMessageSocketEventSerializer = {};
    MessageCreatedSocketEventPayloadDeserializer m_messageCreatedSocketEventPayloadDeserializer = {};

    Logger& m_logger;
};
