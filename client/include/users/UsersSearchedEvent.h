#pragma once

#include "views/UserView.h"

#include <vector>

struct UsersSearchedEvent
{
    UsersSearchedEvent() = default;
    UsersSearchedEvent(const std::vector<UserView>& users)
        : users(users)
    {}

    std::vector<UserView> users = {};
};
