#pragma once

#include "models/User.h"

#include <string>

struct SocketEventContext
{
    int clientSocket = -1;
    std::string serializedPayload = "";
    // NOTE: Set by the authentication middleware, stays null on routes that do not require it
    const User* user = nullptr;
};
