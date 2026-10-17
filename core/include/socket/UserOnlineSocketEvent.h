#pragma once

#include "socket/SocketEvent.h"
#include "views/UserView.h"

#include <string>

struct UserOnlineSocketEventPayload
{
    UserOnlineSocketEventPayload() = default;
    UserOnlineSocketEventPayload(const UserView& user) : user(user)
    {}

    UserView user = {};
};

struct UserOnlineSocketEvent : public SocketEvent<UserOnlineSocketEventPayload>
{
    UserOnlineSocketEvent(UserOnlineSocketEventPayload payload) :  SocketEvent(SocketEventName::USER_ONLINE, payload)
    {}
};
