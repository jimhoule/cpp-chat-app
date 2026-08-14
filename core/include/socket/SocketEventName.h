#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
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

    // Conversations
    CLOSE_CONVERSATION,
    CONVERSATION_CLOSED,
    OPEN_CONVERSATION,
    CONVERSATION_OPENED,
    FIND_ALL_OPEN_CONVERSATIONS_BY_USER_ID,
    ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID,
    FIND_CONVERSATION_BY_USER_IDS,
    CONVERSATION_FOUND_BY_USER_IDS,

    // Messages
    CREATE_MESSAGE,
    MESSAGE_CREATED,
};

/**
 * NOTES:
 *  - Makes the enum travel as a string instead of its position, so adding a value in the middle cannot silently change the meaning of the ones after it
 *  - The strings below are the protocol, ConvertSocketEventNameToString is for logs only
 *  - An unknown name falls back to the first pair instead of throwing, which is what lets Json::GetEnum stay non throwing
 */
NLOHMANN_JSON_SERIALIZE_ENUM(SocketEventName, {
    { SocketEventName::ERROR, "ERROR" },
    
    // Auth
    { SocketEventName::LOGIN, "LOGIN" },
    { SocketEventName::LOGGEDIN, "LOGGEDIN" },
    { SocketEventName::REGISTER, "REGISTER" },
    { SocketEventName::REGISTERED, "REGISTERED" },
    { SocketEventName::USER_AUTHENTICATED, "USER_AUTHENTICATED" },

    // Conversations
    { SocketEventName::CLOSE_CONVERSATION, "CLOSE_CONVERSATION" },
    { SocketEventName::CONVERSATION_CLOSED, "CONVERSATION_CLOSED" },
    { SocketEventName::OPEN_CONVERSATION, "OPEN_CONVERSATION" },
    { SocketEventName::CONVERSATION_OPENED, "CONVERSATION_OPENED" },
    { SocketEventName::FIND_ALL_OPEN_CONVERSATIONS_BY_USER_ID, "FIND_ALL_OPEN_CONVERSATIONS_BY_USER_ID" },
    { SocketEventName::ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID, "ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID" },
    { SocketEventName::FIND_CONVERSATION_BY_USER_IDS, "FIND_CONVERSATION_BY_USER_IDS" },
    { SocketEventName::CONVERSATION_FOUND_BY_USER_IDS, "CONVERSATION_FOUND_BY_USER_IDS" },

    // Messages
    { SocketEventName::CREATE_MESSAGE, "CREATE_MESSAGE" },
    { SocketEventName::MESSAGE_CREATED, "MESSAGE_CREATED" },
});

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
