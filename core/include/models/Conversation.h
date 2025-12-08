#pragma once

#include "Message.h"
#include "User.h"

#include <ctime>
#include <string>
#include <vector>

struct Conversation
{
    std::string ID;
    std::vector<Message> Messages;
    std::vector<User> Users;
    std::time_t CreatedAt;
};
