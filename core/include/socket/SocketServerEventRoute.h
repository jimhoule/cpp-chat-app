#pragma once

#include "socket/SocketServerEventHandler.h"
#include "socket/SocketServerEventMiddleware.h"

#include <vector>

struct SocketServerEventRoute
{
    std::vector<SocketServerEventMiddleware> middlewares = {};
    SocketServerEventHandler handler = nullptr;
};
