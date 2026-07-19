#pragma once

#include "auth/AuthHandler.h"

class SocketServer;
class UsersService;

class AuthModule
{
public:
    AuthModule(SocketServer& socketServer, UsersService& usersService);

    // Deletes move constructor and assign operator
    AuthModule(const AuthModule&) = delete;
    AuthModule& operator=(const AuthModule&) = delete;

    AuthService& GetService();

private:
    AuthHandler m_authHandler;
    AuthService m_authService;
};
