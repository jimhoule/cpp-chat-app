#pragma once

#include "models/User.h"
#include "socket/SocketEvent.h"

#include <string>

struct UserOnlineSocketEventPayload
{
    UserOnlineSocketEventPayload() = default;
    UserOnlineSocketEventPayload(const User& user) : user(user)
    {}

    User user = {};
};

struct UserOnlineSocketEvent : public SocketEvent<UserOnlineSocketEventPayload>
{
    UserOnlineSocketEvent(UserOnlineSocketEventPayload payload) :  SocketEvent(SocketEventName::USER_ONLINE, payload)
    {}
};
