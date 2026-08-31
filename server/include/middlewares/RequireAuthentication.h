#pragma once

#include "socket/SocketServer.h"

SocketServer::EventMiddleware RequireAuthentication(SocketServer& socketServer);
