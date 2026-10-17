#pragma once

#include "views/UserView.h"

// NOTE: Sent when another user comes online
struct UserOnlineEvent
{
    UserOnlineEvent() = default;
    UserOnlineEvent(const UserView& user)
        : user(user)
    {}

    UserView user = {};
};
