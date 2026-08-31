#pragma once

#include "socket/SocketServer.h"

SocketServer::EventErrorHandler HandleErrors(SocketServer& socketServer, Logger& logger);
