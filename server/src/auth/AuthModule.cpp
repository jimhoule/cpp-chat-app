#include "auth/AuthModule.h"

#include "socket/SocketServer.h"
#include "middlewares/RequireAuthentication.h"

// **********
// * PUBLIC *
// **********
AuthModule::AuthModule(SocketServer& socketServer,  SessionsService& sessionsService, UsersService& usersService)
    : m_authServiceLogger("AUTH", "server/src/auth/AuthService")
    , m_authHandlerLogger("AUTH", "server/src/auth/AuthHandler")
    , m_authService(sessionsService, usersService, m_authServiceLogger)
    , m_authHandler(socketServer, m_authService, m_authHandlerLogger)
{
    socketServer.On(
        SocketEventName::AUTHENTICATE,
        m_authHandler,
        &AuthHandler::HandleAuthenticate
    );   
    socketServer.On(
        SocketEventName::LOGIN,
        m_authHandler,
        &AuthHandler::HandleLogin
    );
    socketServer.On(
        SocketEventName::LOGOUT,
        { RequireAuthentication(socketServer) }, 
        m_authHandler,
        &AuthHandler::HandleLogout
    );
	socketServer.On(
        SocketEventName::REGISTER,
        m_authHandler,
        &AuthHandler::HandleRegister
    );
}

AuthService& AuthModule::GetService()
{
    return m_authService;
}
