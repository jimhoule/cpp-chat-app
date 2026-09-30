#pragma once

#include "models/User.h"

struct AuthenticatedEvent
{
    AuthenticatedEvent() = default;
    AuthenticatedEvent(const User& user)
        : user(user)
    {}

    User user = {};
};
