#pragma once

#include "deserializer/CreateMessageSocketEventPayloadDeserializer.h"
#include "serializer/MessageCreatedSocketEventSerializer.h"
#include "socket/SocketServer.h"

// Forward declarations
class MessagesService;
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

    MessageCreatedSocketEventSerializer m_messageCreatedSocketEventSerializer = {};
    CreateMessageSocketEventPayloadDeserializer m_createMessageSocketEventPayloadDeserializer = {};
};
