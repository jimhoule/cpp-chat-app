#pragma once

#include <ctime>
#include <string>

struct MessageView
{
    std::string id;
    std::string conversationId;
    std::string senderId;
    std::string text;
    std::time_t createdAt;
};
