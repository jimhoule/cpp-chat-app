#pragma once

#include "messages/MessagesHandler.h"

class SocketServer;

class MessagesModule
{
public:
    MessagesModule(SocketServer& socketServer);

    // Deletes move constructor and assign operator
    MessagesModule(const MessagesModule&) = delete;
    MessagesModule& operator=(const MessagesModule&) = delete;

    MessagesService& GetService();

private:
    MessagesHandler m_messagesHandler;
    MessagesService m_messagesService;
};
