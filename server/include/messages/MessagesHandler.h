#pragma once

#include "deserializer/CreateMessageSocketEventPayloadDeserializer.h"
#include "messages/MessagesService.h"
#include "serializer/MessageCreatedSocketEventSerializer.h"
#include "socket/SocketServer.h"

class MessagesHandler
{
public:
    MessagesHandler(SocketServer& socketServer, const MessagesService& messagesService);

    SocketServerEventHandler GetCreateMessageHandler();

private:
    SocketServer& m_socketServer;
    MessagesService m_messagesService;

    CreateMessageSocketEventPayloadDeserializer m_createMessageSocketEventPayloadDeserializer = {};

    MessageCreatedSocketEventSerializer m_messageCreatedSocketEventSerializer = {};
};
