#pragma once

#include <iostream>

enum class SocketEventName
{
    ERROR,
    LOGIN,
    LOGGEDIN,
    REGISTER,
    REGISTERED,
};

// Overloads the stream insertion operator (<<) for the SocketEventName enum class
static std::ostream& operator<<(std::ostream& Os, const SocketEventName& EventName)
{
    switch (EventName)
    {
        case SocketEventName::ERROR:
            Os << "ERROR";
            break;

        case SocketEventName::LOGIN:
            Os << "LOGIN";
            break;

        case SocketEventName::LOGGEDIN:
            Os << "LOGGEDIN";
            break;

        case SocketEventName::REGISTER:
            Os << "REGISTER";
            break;

        case SocketEventName::REGISTERED:
            Os << "REGISTERED";
            break;

        // NOTE: Handles cases where the enum value might be out of range
        default:
            Os << "Unknown socket event name";
            break;
    }

    // NOTE: Enables chaining
    return Os;
};
