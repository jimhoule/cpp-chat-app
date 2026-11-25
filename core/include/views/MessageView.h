#pragma once

#include <cstdint>
#include <ctime>
#include <string>

struct MessageView
{
    std::string id;
    std::string conversationId;
    std::uint64_t position;
    std::string senderId;
    std::string text;
    std::time_t createdAt;
};
