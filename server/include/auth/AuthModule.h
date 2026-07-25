#pragma once

#include "log/Logger.h"
#include "auth/AuthService.h"
#include "auth/AuthHandler.h"

class SocketServer;
class SessionsService;
class UsersService;

class AuthModule
{
public:
    AuthModule(SocketServer& socketServer, SessionsService& sessionsService, UsersService& usersService);

    // Deletes move constructor and assign operator
    AuthModule(const AuthModule&) = delete;
    AuthModule& operator=(const AuthModule&) = delete;

    AuthService& GetService();

private:
/**
     * NOTES
     *  - Declaration order is initialization order because these loggers must be declared before the service and the handler that borrow them
     *  - Their order relative to each other does not matter, they are sorted by dependency depth for readability only
     */
    Logger m_authServiceLogger;
    Logger m_authHandlerLogger;

    AuthHandler m_authHandler;
    AuthService m_authService;
};
