#pragma once

#include <iostream>
#include <string>

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

std::string ConvertSocketEventNameToString(SocketEventName socketEventName);

// NOTE: Overloads the stream insertion operator (<<) for the SocketEventName enum class
std::ostream& operator<<(std::ostream& Os, SocketEventName SocketEventName);

// NOTE:  Overloads concatenation operator for std::string on the left side
std::string operator+(const std::string& string, SocketEventName socketEventName);
// NOTE: Overloads concatenation operator for std::string on the right side
std::string operator+(SocketEventName socketEventName, const std::string& string);
// NOTE: Overloads concatenation operator for c style string on the left side
std::string operator+(const char* cString, SocketEventName socketEventName);
// NOTE: Overloads concatenation operator for c style string on the right side
std::string operator+(SocketEventName socketEventName, const char* cString);
