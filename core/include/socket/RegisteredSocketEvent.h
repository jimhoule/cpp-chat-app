#pragma once

#include "SocketEvent.h"

#include <string>

struct RegisteredSocketEventPayload
{
    RegisteredSocketEventPayload() = default;
    RegisteredSocketEventPayload(const std::string& sessionId) : sessionId(sessionId)
    {}

    std::string sessionId;
};

struct RegisteredSocketEvent : public SocketEvent<RegisteredSocketEventPayload>
{
    RegisteredSocketEvent(RegisteredSocketEventPayload payload) :  SocketEvent(SocketEventName::REGISTERED, payload)
    {}
};
