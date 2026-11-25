#pragma once

#include <cstdint>
#include <string>

struct ConversationUser
{
    bool isOpen;
    // NOTE: Send order of last message read (0 = nothing read yet)
    std::uint64_t lastReadMessagePosition;
    std::string userId;
};
