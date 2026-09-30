#pragma once

#include <nlohmann/json.hpp>
#include <string>

enum class SocketErrorCode
{
    INTERNAL,
    INVALID_PAYLOAD,
    NOT_AUTHENTICATED,
    INVALID_CREDENTIALS,
    ALREADY_EXISTS,
    NOT_FOUND,
    UNKNOWN_SESSION,
    EXPIRED_SESSION
};

/**
 * NOTES:
 *  - The strings below are the protocol, ConvertSocketErrorCodeToString is for logs only
 *  - An unknown code falls back to the first pair, INTERNAL
 */
NLOHMANN_JSON_SERIALIZE_ENUM(SocketErrorCode, {
    { SocketErrorCode::INTERNAL, "INTERNAL" },
    { SocketErrorCode::INVALID_PAYLOAD, "INVALID_PAYLOAD" },
    { SocketErrorCode::NOT_AUTHENTICATED, "NOT_AUTHENTICATED" },
    { SocketErrorCode::INVALID_CREDENTIALS, "INVALID_CREDENTIALS" },
    { SocketErrorCode::ALREADY_EXISTS, "ALREADY_EXISTS" },
    { SocketErrorCode::NOT_FOUND, "NOT_FOUND" },
    { SocketErrorCode::UNKNOWN_SESSION, "UNKNOWN_SESSION" },
    { SocketErrorCode::EXPIRED_SESSION, "EXPIRED_SESSION" },
});

std::string ConvertSocketErrorCodeToString(SocketErrorCode socketErrorCode);
