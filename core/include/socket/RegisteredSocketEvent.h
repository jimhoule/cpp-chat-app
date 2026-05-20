#pragma once

#include "SocketEvent.h"

#include <string>

struct RegisteredSocketEventPayload
{
    RegisteredSocketEventPayload() = default;
    RegisteredSocketEventPayload(const std::string& AccessToken) : AccessToken(AccessToken)
    {}

    std::string AccessToken;
};

struct RegisteredSocketEvent : public SocketEvent<RegisteredSocketEventPayload>
{
    RegisteredSocketEvent(RegisteredSocketEventPayload RegisteredSocketEventPayload) :  SocketEvent(SocketEventName::REGISTERED, RegisteredSocketEventPayload)
    {}
};
