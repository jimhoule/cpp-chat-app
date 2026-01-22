#pragma once

#include <functional>
#include <string>

using SocketServerEventHandler = std::function<void(const std::string&, int)>;
