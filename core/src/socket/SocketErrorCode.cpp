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
