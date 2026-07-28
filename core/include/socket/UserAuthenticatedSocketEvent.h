#pragma once

#include "models/User.h"
#include "socket/SocketEvent.h"

#include <string>

struct UserAuthenticatedSocketEventPayload
{
    UserAuthenticatedSocketEventPayload() = default;
    UserAuthenticatedSocketEventPayload(const User& user) : user(user)
    {}

    User user = {};
};

struct UserAuthenticatedSocketEvent : public SocketEvent<UserAuthenticatedSocketEventPayload>
{
    UserAuthenticatedSocketEvent(UserAuthenticatedSocketEventPayload payload) :  SocketEvent(SocketEventName::USER_AUTHENTICATED, payload)
    {}
};
