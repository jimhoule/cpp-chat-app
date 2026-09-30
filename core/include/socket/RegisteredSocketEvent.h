#pragma once

#include "models/User.h"
#include "SocketEvent.h"

#include <string>

struct RegisteredSocketEventPayload
{
    RegisteredSocketEventPayload() = default;
    RegisteredSocketEventPayload(const std::string& sessionId, const User& user) : sessionId(sessionId), user(user)
    {}

    std::string sessionId;
    User user = {};
};

struct RegisteredSocketEvent : public SocketEvent<RegisteredSocketEventPayload>
{
    RegisteredSocketEvent(RegisteredSocketEventPayload payload) :  SocketEvent(SocketEventName::REGISTERED, payload)
    {}
};
