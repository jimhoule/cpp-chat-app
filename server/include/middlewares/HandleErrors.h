#pragma once

#include "socket/SocketServerEventErrorHandler.h"

// Forward declarations
class Logger;
class SocketServer;

SocketServerEventErrorHandler HandleErrors(SocketServer& socketServer, Logger& logger);
