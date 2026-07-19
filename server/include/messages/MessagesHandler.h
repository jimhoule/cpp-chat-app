#pragma once

#include "deserializer/CreateMessageSocketEventPayloadDeserializer.h"
#include "messages/MessagesService.h"
#include "serializer/MessageCreatedSocketEventSerializer.h"
#include "socket/SocketServerEventHandler.h"

class SocketServer;

class MessagesHandler
{
public:
    MessagesHandler(SocketServer& socketServer, MessagesService& messagesService);

    SocketServerEventHandler GetCreateMessageHandler();

private:
    SocketServer& m_socketServer;
    MessagesService& m_messagesService;

    MessageCreatedSocketEventSerializer m_messageCreatedSocketEventSerializer = {};
    CreateMessageSocketEventPayloadDeserializer m_createMessageSocketEventPayloadDeserializer = {};
};
