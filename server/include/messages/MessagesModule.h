#pragma once

#include "log/Logger.h"
#include "messages/MessagesService.h"
#include "messages/MessagesHandler.h"

// Forward declarations
class SocketServer;
class UuidService;

class MessagesModule
{
public:
    MessagesModule(SocketServer& socketServer, UuidService& uuidService);

    // Deletes move constructor and assign operator
    MessagesModule(const MessagesModule&) = delete;
    MessagesModule& operator=(const MessagesModule&) = delete;

    MessagesService& GetService();

private:
    /**
     * NOTES
     *  - Declaration order is initialization order because these loggers must be declared before the service and the handler that borrow them
     *  - Their order relative to each other does not matter, they are sorted by dependency depth for readability only
     */
    Logger m_messagesRepositoryLogger;
    Logger m_messagesServiceLogger;
    Logger m_messagesHandlerLogger;

    // NOTE: m_messagesService must be declared before m_messagesHandler because it is borrowed by it
    MessagesService m_messagesService;
    MessagesHandler m_messagesHandler;
};
