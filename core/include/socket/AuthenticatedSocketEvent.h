#pragma once

#include "socket/SocketEvent.h"
#include "views/UserView.h"

#include <string>

struct AuthenticatedSocketEventPayload
{
    AuthenticatedSocketEventPayload() = default;
    AuthenticatedSocketEventPayload(const UserView& user) : user(user)
    {}

    UserView user = {};
};

struct AuthenticatedSocketEvent : public SocketEvent<AuthenticatedSocketEventPayload>
{
    AuthenticatedSocketEvent(AuthenticatedSocketEventPayload payload) :  SocketEvent(SocketEventName::AUTHENTICATED, payload)
    {}
};
