#pragma once

#include "socket/SocketEvent.h"
#include "views/UserView.h"

#include <string>

struct RegisteredSocketEventPayload
{
    RegisteredSocketEventPayload() = default;
    RegisteredSocketEventPayload(const std::string& sessionId, const UserView& user) : sessionId(sessionId), user(user)
    {}

    std::string sessionId;
    UserView user = {};
};

struct RegisteredSocketEvent : public SocketEvent<RegisteredSocketEventPayload>
{
    RegisteredSocketEvent(RegisteredSocketEventPayload payload) :  SocketEvent(SocketEventName::REGISTERED, payload)
    {}
};
