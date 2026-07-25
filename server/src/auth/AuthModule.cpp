#include "auth/AuthModule.h"

#include "socket/SocketServer.h"

AuthModule::AuthModule(SocketServer& socketServer,  SessionsService& sessionsService, UsersService& usersService)
    : m_authServiceLogger("AUTH", "server/src/auth/AuthService")
    , m_authHandlerLogger("AUTH", "server/src/auth/AuthHandler")
    , m_authService(sessionsService, usersService, m_authServiceLogger)
    , m_authHandler(socketServer, m_authService, m_authHandlerLogger)
{
    socketServer.On(SocketEventName::LOGIN, m_authHandler.GetLoginHandler());
	socketServer.On(SocketEventName::REGISTER, m_authHandler.GetRegisterHandler());
}

AuthService& AuthModule::GetService()
{
    return m_authService;
}
