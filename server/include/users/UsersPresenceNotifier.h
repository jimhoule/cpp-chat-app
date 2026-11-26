#pragma once

#include "serializer/UserOfflineSocketEventSerializer.h"
#include "serializer/UserOnlineSocketEventSerializer.h"
#include "socket/SocketServer.h"

// Forward declarations
class Logger;
class UsersService;

class UsersPresenceNotifier
{
public:
    UsersPresenceNotifier(SocketServer& socketServer, UsersService& usersService, Logger& logger);

    void HandleUserBound(const SocketServer::UserBoundContext& context);
    void HandleUserUnbound(const SocketServer::UserUnboundContext& context);

private:
    SocketServer& m_socketServer;

    UsersService& m_usersService;

    Logger& m_logger;

    UserOfflineSocketEventSerializer m_userOfflineSocketEventSerializer = {};
    UserOnlineSocketEventSerializer m_userOnlineSocketEventSerializer = {};
};
