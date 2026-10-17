#pragma once

#include "views/ConversationUserView.h"

#include <ctime>
#include <string>
#include <vector>

struct ConversationView
{
    std::string id;
    std::vector<ConversationUserView> users;
    std::time_t createdAt;
};
