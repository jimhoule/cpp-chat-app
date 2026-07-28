#pragma once

#include "socket/SocketEventContext.h"

#include <functional>

using SocketServerEventMiddleware = std::function<void(SocketEventContext&)>;