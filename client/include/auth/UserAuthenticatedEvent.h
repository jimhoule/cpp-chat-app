#pragma once

#include "models/User.h"

// NOTE: Sent when another user comes online
struct UserAuthenticatedEvent
{
    UserAuthenticatedEvent() = default;
    UserAuthenticatedEvent(const User& user)
        : user(user)
    {}

    User user = {};
};
