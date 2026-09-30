#pragma once

#include "models/User.h"
#include "socket/SocketEvent.h"

#include <string>

struct AuthenticatedSocketEventPayload
{
    AuthenticatedSocketEventPayload() = default;
    AuthenticatedSocketEventPayload(const User& user) : user(user)
    {}

    User user = {};
};

struct AuthenticatedSocketEvent : public SocketEvent<AuthenticatedSocketEventPayload>
{
    AuthenticatedSocketEvent(AuthenticatedSocketEventPayload payload) :  SocketEvent(SocketEventName::AUTHENTICATED, payload)
    {}
};
