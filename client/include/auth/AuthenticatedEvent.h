#pragma once

#include "views/UserView.h"

struct AuthenticatedEvent
{
    AuthenticatedEvent() = default;
    AuthenticatedEvent(const UserView& user)
        : user(user)
    {}

    UserView user = {};
};
