#pragma once

#include "socket/SocketServerEventMiddleware.h"

// Forward declarations
class SocketServer;

SocketServerEventMiddleware RequireAuthentication(SocketServer& socketServer);
