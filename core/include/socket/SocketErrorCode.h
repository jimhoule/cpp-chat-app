#pragma once

#include <string>

enum class SocketErrorCode
{
    INTERNAL,
    INVALID_PAYLOAD,
    NOT_AUTHENTICATED,
    INVALID_CREDENTIALS,
    ALREADY_EXISTS,
    NOT_FOUND,
};

std::string ConvertSocketErrorCodeToString(SocketErrorCode socketErrorCode);
