#pragma once

#include "SocketEvent.h"

#include <string>

struct UserOfflineSocketEventPayload
{
    UserOfflineSocketEventPayload() = default;
    UserOfflineSocketEventPayload(const std::string& userId) : userId(userId)
    {}

    std::string userId;
};

struct UserOfflineSocketEvent : public SocketEvent<UserOfflineSocketEventPayload>
{
    UserOfflineSocketEvent(UserOfflineSocketEventPayload payload) :  SocketEvent(SocketEventName::USER_OFFLINE, payload)
    {}
};
