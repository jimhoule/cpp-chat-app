#pragma once

#include "models/User.h"

struct SocketConnection
{
    int clientSocket = -1;
    std::optional<User> user = std::nullopt;
    std::string readBuffer = "";
};
