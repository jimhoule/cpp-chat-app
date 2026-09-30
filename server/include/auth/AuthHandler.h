#pragma once

#include "auth/AuthService.h"
#include "deserializer/AuthenticateSocketEventPayloadDeserializer.h"
#include "deserializer/LoginSocketEventPayloadDeserializer.h"
#include "deserializer/LogoutSocketEventPayloadDeserializer.h"
#include "deserializer/RegisterSocketEventPayloadDeserializer.h"
#include "serializer/AuthenticatedSocketEventSerializer.h"
#include "serializer/LoggedinSocketEventSerializer.h"
#include "serializer/LoggedoutSocketEventSerializer.h"
#include "serializer/RegisteredSocketEventSerializer.h"
#include "serializer/UserOfflineSocketEventSerializer.h"
#include "serializer/UserOnlineSocketEventSerializer.h"
#include "socket/SocketErrorCode.h"
#include "socket/SocketServer.h"

// Forward declarations
class Logger;

class AuthHandler
{
public:
    AuthHandler(SocketServer& socketServer, AuthService& authService, Logger& logger);

    SocketServer::EventHandler GetAuthenticateHandler();
    SocketServer::EventHandler GetLoginHandler();
    SocketServer::EventHandler GetLogoutHandler();
    SocketServer::EventHandler GetRegisterHandler();

private:
    SocketServer& m_socketServer;
    AuthService& m_authService;

    // NOTE: Borrowed from the module, which owns it and outlives this handler
    Logger& m_logger;

    AuthenticatedSocketEventSerializer m_authenticatedSocketEventSerializer = {};
    AuthenticateSocketEventPayloadDeserializer m_authenticateSocketEventPayloadDeserializer = {};

    LoggedinSocketEventSerializer m_loggedinSocketEventSerializer = {};
    LoginSocketEventPayloadDeserializer m_loginSocketEventPayloadDeserializer = {};

    LoggedoutSocketEventSerializer m_loggedoutSocketEventSerializer = {};
    LogoutSocketEventPayloadDeserializer m_logoutSocketEventPayloadDeserializer = {};

    RegisteredSocketEventSerializer m_registeredSocketEventSerializer = {};
    RegisterSocketEventPayloadDeserializer m_registerSocketEventPayloadDeserializer = {};

    UserOfflineSocketEventSerializer m_userOfflineSocketEventSerializer = {};

    UserOnlineSocketEventSerializer m_userOnlineSocketEventSerializer = {};

    void SendUserOfflineSocketEvent(int clientSocket, const std::string& userId);
    void SendUserOnlineSocketEvent(int clientSocket, const User& user);
    SocketErrorCode ConvertAuthResultCodeToSocketErrorCode(AuthService::AuthResultCode authResultCode);
};
