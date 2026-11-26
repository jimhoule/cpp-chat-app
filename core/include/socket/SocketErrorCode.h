#pragma once

#include <nlohmann/json.hpp>
#include <ostream>
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

// NOTE: Overloads the stream insertion operator (<<)
std::ostream& operator<<(std::ostream& Os, SocketErrorCode socketErrorCode);

// NOTE:  Overloads concatenation operator for std::string on the left side
std::string operator+(const std::string& string, SocketErrorCode socketErrorCode);
// NOTE: Overloads concatenation operator for std::string on the right side
std::string operator+(SocketErrorCode socketErrorCode, const std::string& string);
// NOTE: Overloads concatenation operator for c style string on the left side
std::string operator+(const char* cString, SocketErrorCode socketErrorCode);
// NOTE: Overloads concatenation operator for c style string on the right side
std::string operator+(SocketErrorCode socketErrorCode, const char* cString);
