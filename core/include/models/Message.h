#pragma once

#include <string>

struct Message
{
    std::string id;
    std::string conversationId;
    std::string senderId;
    std::string text;
    std::time_t createdAt;
};
