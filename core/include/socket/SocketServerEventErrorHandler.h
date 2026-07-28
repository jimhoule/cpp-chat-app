#pragma once

#include "socket/SocketEventContext.h"

#include <exception>
#include <functional>

using SocketServerEventErrorHandler = std::function<void(const std::exception&, SocketEventContext&)>;