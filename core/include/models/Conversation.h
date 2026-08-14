#pragma once

#include "models/ConversationUser.h"

#include <ctime>
#include <string>
#include <vector>

struct Conversation
{
    std::string id;
    std::vector<ConversationUser> users;
    std::time_t createdAt;
};
