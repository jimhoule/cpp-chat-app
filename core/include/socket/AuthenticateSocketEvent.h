#pragma once

#include "SocketEvent.h"

#include <string>

struct AuthenticateSocketEventPayload
{
    AuthenticateSocketEventPayload() = default;
    AuthenticateSocketEventPayload(const std::string& sessionId) : sessionId(sessionId)
    {}

    std::string sessionId;
};

struct AuthenticateSocketEvent : public SocketEvent<AuthenticateSocketEventPayload>
{
    AuthenticateSocketEvent(AuthenticateSocketEventPayload payload) :  SocketEvent(SocketEventName::AUTHENTICATE, payload)
    {}
};
