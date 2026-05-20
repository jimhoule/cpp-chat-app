#pragma once

#include "models/User.h"
#include "socket/SocketEvent.h"

#include <string>

struct UserAuthenticatedSocketEventPayload
{
    UserAuthenticatedSocketEventPayload() = default;
    UserAuthenticatedSocketEventPayload(const User& User) : User(User)
    {}

    User User = {};
};

struct UserAuthenticatedSocketEvent : public SocketEvent<UserAuthenticatedSocketEventPayload>
{
    UserAuthenticatedSocketEvent(UserAuthenticatedSocketEventPayload Payload) :  SocketEvent(SocketEventName::USER_AUTHENTICATED, Payload)
    {}
};
