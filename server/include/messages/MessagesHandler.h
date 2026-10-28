#pragma once

#include "deserializer/CreateMessageSocketEventPayloadDeserializer.h"
#include "messages/MessagesService.h"
#include "serializer/ConversationReopenedSocketEventSerializer.h"
#include "serializer/MessageCreatedSocketEventSerializer.h"
#include "socket/SocketErrorCode.h"
#include "socket/SocketServer.h"

// Forward declarations
class Logger;

class MessagesHandler
{
public:
    MessagesHandler(SocketServer& socketServer, MessagesService& messagesService, Logger& logger);

    SocketServer::EventHandler GetCreateMessageHandler();

private:
    SocketServer& m_socketServer;
    MessagesService& m_messagesService;

    // NOTE: Borrowed from the module, which owns it and outlives this handler
    Logger& m_logger;

    ConversationReopenedSocketEventSerializer m_conversationReopenedSocketEventSerializer = {};

    MessageCreatedSocketEventSerializer m_messageCreatedSocketEventSerializer = {};
    CreateMessageSocketEventPayloadDeserializer m_createMessageSocketEventPayloadDeserializer = {};

    SocketErrorCode ConvertMessagesResultCodeToSocketErrorCode(MessagesService::MessagesResultCode messagesResultCode);
};
