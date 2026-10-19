#pragma once

#include "deserializer/SearchUsersSocketEventPayloadDeserializer.h"
#include "serializer/UsersSearchedSocketEventSerializer.h"
#include "socket/SocketServer.h"

// Forward declarations
class UsersService;
class Logger;

class UsersHandler
{
public:
    UsersHandler(SocketServer& socketServer, UsersService& usersService, Logger& logger);

    SocketServer::EventHandler GetSearchUsersHandler();

private:
    SocketServer& m_socketServer;
    UsersService& m_usersService;

    // NOTE: Borrowed from the module, which owns it and outlives this handler
    Logger& m_logger;

    UsersSearchedSocketEventSerializer m_usersSearchedSocketEventSerializer = {};
    SearchUsersSocketEventPayloadDeserializer m_searchUsersSocketEventPayloadDeserializer = {};
};
