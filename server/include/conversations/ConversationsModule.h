#pragma once

#include "conversations/ConversationsService.h"
#include "conversations/ConversationsHandler.h"
#include "log/Logger.h"

// Forward declarations
class SocketServer;
class UsersService;
class UuidService;

class ConversationsModule
{
public:
    ConversationsModule(SocketServer& socketServer, UsersService& usersService, UuidService& uuidService);

    // Deletes move constructor and assign operator
    ConversationsModule(const ConversationsModule&) = delete;
    ConversationsModule& operator=(const ConversationsModule&) = delete;

    ConversationsService& GetService();

private:
    /**
     * NOTES
     *  - Declaration order is initialization order because these loggers must be declared before the service and the handler that borrow them
     *  - Their order relative to each other does not matter, they are sorted by dependency depth for readability only
     */
    Logger m_conversationsRepositoryLogger;
    Logger m_conversationsServiceLogger;
    Logger m_conversationsHandlerLogger;

    // NOTE: m_conversationsService must be declared before m_conversationsService because it is borrowed by it
    ConversationsService m_conversationsService;
    ConversationsHandler m_conversationsHandler;
};
