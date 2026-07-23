#pragma once

#include <string>

struct Session
{
    std::string id;
    std::string userId;
    std::time_t createdAt;
    std::time_t expiredAt;
};
