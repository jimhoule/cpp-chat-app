#pragma once

#include "deserializer/LoginSocketEventPayloadDeserializer.h"
#include "deserializer/RegisterSocketEventPayloadDeserializer.h"
#include "serializer/LoggedinSocketEventSerializer.h"
#include "serializer/RegisteredSocketEventSerializer.h"
#include "serializer/UserAuthenticatedSocketEventSerializer.h"
#include "socket/SocketServerEventHandler.h"

// Forward declarations
class SocketServer;
class AuthService;
class Logger;

class AuthHandler
{
public:
    AuthHandler(SocketServer& socketServer, AuthService& authService, Logger& logger);

    SocketServerEventHandler GetLoginHandler();
    SocketServerEventHandler GetRegisterHandler();

private:
    SocketServer& m_socketServer;
    AuthService& m_authService;

    // NOTE: Borrowed from the module, which owns it and outlives this handler
    Logger& m_logger;

    LoggedinSocketEventSerializer m_loggedinSocketEventSerializer = {};
    LoginSocketEventPayloadDeserializer m_loginSocketEventPayloadDeserializer = {};

    RegisteredSocketEventSerializer m_registeredSocketEventSerializer = {};
    RegisterSocketEventPayloadDeserializer m_registerSocketEventPayloadDeserializer = {};

    UserAuthenticatedSocketEventSerializer m_userAuthenticatedSocketEventSerializer = {};

    void SendUserAuthenticatedSocketEvent(int clientSocket, const User& user);
};
