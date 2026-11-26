#include "socket/SocketErrorCode.h"

std::string ConvertSocketErrorCodeToString(SocketErrorCode socketErrorCode)
{
    switch (socketErrorCode)
    {
        case SocketErrorCode::INTERNAL:
            return "INTERNAL";

        case SocketErrorCode::INVALID_PAYLOAD:
            return "INVALID PAYLOAD";

        case SocketErrorCode::NOT_AUTHENTICATED:
            return "NOT AUTHENTICATED";

        case SocketErrorCode::INVALID_CREDENTIALS:
            return "INVALID CREDENTIALS";

        case SocketErrorCode::ALREADY_EXISTS:
            return "ALREADY EXISTS";

        case SocketErrorCode::NOT_FOUND:
            return "NOT FOUND";

        case SocketErrorCode::UNKNOWN_SESSION:
            return "UNKNOWN SESSION";

        case SocketErrorCode::EXPIRED_SESSION:
            return "EXPIRED SESSION";

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return "Unknown socket error code";
    }
}

// NOTE: Overloads the stream insertion operator (<<)
std::ostream& operator<<(std::ostream& Os, SocketErrorCode socketErrorCode)
{
    Os << ConvertSocketErrorCodeToString(socketErrorCode);

    // NOTE: Enables chaining
    return Os;
}

// NOTE: Overloads concatenation operator for std::string on the left side
std::string operator+(const std::string& string, SocketErrorCode socketErrorCode)
{
    return string + ConvertSocketErrorCodeToString(socketErrorCode);
}

// NOTE: Overloads concatenation operator for std::string on the right side
std::string operator+(SocketErrorCode socketErrorCode, const std::string& string)
{
    return ConvertSocketErrorCodeToString(socketErrorCode) + string;
}

// NOTE: Overloads concatenation operator for c style string on the left side
std::string operator+(const char* cString, SocketErrorCode socketErrorCode)
{
    return std::string(cString) + ConvertSocketErrorCodeToString(socketErrorCode);
}

// NOTE: Overloads concatenation operator for c style string on the right side
std::string operator+(SocketErrorCode socketErrorCode, const char* cString)
{
    return ConvertSocketErrorCodeToString(socketErrorCode) + cString;
}
