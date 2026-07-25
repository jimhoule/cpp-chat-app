#include "auth/AuthModule.h"

#include "socket/SocketServer.h"

AuthModule::AuthModule(SocketServer& socketServer,  SessionsService& sessionsService, UsersService& usersService) : m_authService(sessionsService, usersService), m_authHandler(socketServer, m_authService)
{
    socketServer.On(SocketEventName::LOGIN, m_authHandler.GetLoginHandler());
	socketServer.On(SocketEventName::REGISTER, m_authHandler.GetRegisterHandler());
}

AuthService& AuthModule::GetService()
{
    return m_authService;
}
