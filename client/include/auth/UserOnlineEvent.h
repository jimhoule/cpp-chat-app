#pragma once

#include "models/User.h"

// NOTE: Sent when another user comes online
struct UserOnlineEvent
{
    UserOnlineEvent() = default;
    UserOnlineEvent(const User& user)
        : user(user)
    {}

    User user = {};
};
