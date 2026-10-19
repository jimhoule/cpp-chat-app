#pragma once

#include "socket/SocketEvent.h"
#include "views/UserView.h"

#include <vector>

struct UsersSearchedSocketEventPayload
{
    UsersSearchedSocketEventPayload() = default;
    UsersSearchedSocketEventPayload(const std::vector<UserView>& users)
        : users(users)
    {}

    std::vector<UserView> users;
};

struct UsersSearchedSocketEvent : public SocketEvent<UsersSearchedSocketEventPayload>
{
    UsersSearchedSocketEvent(UsersSearchedSocketEventPayload payload) :  SocketEvent(SocketEventName::USERS_SEARCHED, payload)
    {}
};