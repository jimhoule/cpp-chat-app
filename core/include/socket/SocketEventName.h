#pragma once

#include <iostream>

enum class SocketEventName
{
    ERROR,
    // Auth

    LOGIN,
    LOGGEDIN,
    REGISTER,
    REGISTERED,
    USER_AUTHENTICATED,

    // Messages
    CREATE_MESSAGE,
    MESSAGE_CREATED,
};

// Overloads the stream insertion operator (<<) for the SocketEventName enum class
static std::ostream& operator<<(std::ostream& Os, const SocketEventName& EventName)
{
    switch (EventName)
    {
        case SocketEventName::ERROR:
            Os << "ERROR";
            break;

        // Auth
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

        case SocketEventName::USER_AUTHENTICATED:
            Os << "USER AUTHENTICATED";
            break;

        // Messages
        case SocketEventName::CREATE_MESSAGE:
            Os << "CREATE MESSAGE";
            break;

        case SocketEventName::MESSAGE_CREATED:
            Os << "MESSAGE CREATED";
            break;

        // NOTE: Handles cases where the enum value might be out of range
        default:
            Os << "Unknown socket event name";
            break;
    }

    // NOTE: Enables chaining
    return Os;
};
