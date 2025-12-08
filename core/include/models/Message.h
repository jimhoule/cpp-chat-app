#pragma once

#include <ctime>
#include <string>

struct Message
{
    std::string ID;
    std::string ConversationID;
    std::string SenderID;
    std::string SenderFirstName;
    std::string SenderImageUrl;
    std::string Text;
    std::time_t CreatedAt;
};
