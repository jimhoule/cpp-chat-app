#pragma once

#include "socket/SocketEventContext.h"

#include <functional>
#include <string>

using SocketServerEventHandler = std::function<void(const SocketEventContext&)>;
