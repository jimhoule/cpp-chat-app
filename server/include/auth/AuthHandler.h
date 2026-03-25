#pragma once

#include "auth/AuthService.h"
#include "deserializer/LoginSocketEventPayloadDeserializer.h"
#include "deserializer/RegisterSocketEventPayloadDeserializer.h"
#include "serializer/LoggedinSocketEventSerializer.h"
#include "serializer/RegisteredSocketEventSerializer.h"
#include "serializer/UserAuthenticatedSocketEventSerializer.h"
#include "socket/SocketServer.h"

class AuthHandler
{
public:
    AuthHandler(const SocketServer& SocketServer, const AuthService& AuthService);

    SocketServerEventHandler GetLoginHandler();
    SocketServerEventHandler GetRegisterHandler();

private:
    SocketServer m_SocketServer;
    AuthService m_AuthService;

    LoggedinSocketEventSerializer m_LoggedinSocketEventSerializer = {};
    LoginSocketEventPayloadDeserializer m_LoginSocketEventPayloadDeserializer = {};

    RegisteredSocketEventSerializer m_RegisteredSocketEventSerializer = {};
    RegisterSocketEventPayloadDeserializer m_RegisterSocketEventPayloadDeserializer = {};

    UserAuthenticatedSocketEventSerializer m_UserAuthenticatedSocketEventSerializer = {};

    void SendUserAuthenticatedSocketEvent(int ClientSocket, const std::string& AccessToken);
};
