#include "socket/SocketEventName.h"

std::string ConvertSocketEventNameToString(SocketEventName socketEventName)
{
    switch (socketEventName)
    {
        case SocketEventName::ERROR:
            return "ERROR";

        // Auth
        case SocketEventName::LOGIN:
            return "LOGIN";

        case SocketEventName::LOGGEDIN:
            return "LOGGEDIN";

        case SocketEventName::REGISTER:
            return "REGISTER";

        case SocketEventName::REGISTERED:
            return "REGISTERED";

        case SocketEventName::USER_AUTHENTICATED:
            return "USER AUTHENTICATED";

        // Messages
        case SocketEventName::CREATE_MESSAGE:
            return "CREATE MESSAGE";

        case SocketEventName::MESSAGE_CREATED:
            return "MESSAGE CREATED";

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return "Unknown socket event name";
    }
}

// NOTE: Overloads the stream insertion operator (<<) for the SocketEventName enum class
std::ostream& operator<<(std::ostream& Os, SocketEventName SocketEventName)
{
    Os << ConvertSocketEventNameToString(SocketEventName);

    // NOTE: Enables chaining
    return Os;
}

// NOTE: Overloads concatenation operator for std::string on the left side
std::string operator+(const std::string& string, SocketEventName socketEventName)
{
    return string + ConvertSocketEventNameToString(socketEventName);
}

// NOTE: Overloads concatenation operator for std::string on the right side
std::string operator+(SocketEventName socketEventName, const std::string& string)
{
    return ConvertSocketEventNameToString(socketEventName) + string;
}

// NOTE: Overloads concatenation operator for c style string on the left side
std::string operator+(const char* cString, SocketEventName socketEventName)
{
    return std::string(cString) + ConvertSocketEventNameToString(socketEventName);
}

// NOTE: Overloads concatenation operator for c style string on the right side
std::string operator+(SocketEventName socketEventName, const char* cString)
{
    return ConvertSocketEventNameToString(socketEventName) + cString;
}