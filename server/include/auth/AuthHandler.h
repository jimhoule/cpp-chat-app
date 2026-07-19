#pragma once

#include "auth/AuthService.h"
#include "deserializer/LoginSocketEventPayloadDeserializer.h"
#include "deserializer/RegisterSocketEventPayloadDeserializer.h"
#include "serializer/LoggedinSocketEventSerializer.h"
#include "serializer/RegisteredSocketEventSerializer.h"
#include "serializer/UserAuthenticatedSocketEventSerializer.h"
#include "socket/SocketServerEventHandler.h"

class SocketServer;
class AuthHandler
{
public:
    AuthHandler(SocketServer& socketServer, AuthService& authService);

    SocketServerEventHandler GetLoginHandler();
    SocketServerEventHandler GetRegisterHandler();

private:
    SocketServer& m_socketServer;
    AuthService& m_authService;

    LoggedinSocketEventSerializer m_loggedinSocketEventSerializer = {};
    LoginSocketEventPayloadDeserializer m_loginSocketEventPayloadDeserializer = {};

    RegisteredSocketEventSerializer m_registeredSocketEventSerializer = {};
    RegisterSocketEventPayloadDeserializer m_registerSocketEventPayloadDeserializer = {};

    UserAuthenticatedSocketEventSerializer m_userAuthenticatedSocketEventSerializer = {};

    void SendErrorSocketEvent(int clientSocket);
    void SendUserAuthenticatedSocketEvent(int clientSocket, const User& user);
};
