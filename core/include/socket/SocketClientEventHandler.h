#pragma once

#include <functional>
#include <string>

using SocketClientEventHandler = std::function<void(const std::string&)>;
