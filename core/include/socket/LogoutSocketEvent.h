#pragma once

#include "SocketEvent.h"

#include <string>

struct LogoutSocketEventPayload
{
    LogoutSocketEventPayload() = default;
    LogoutSocketEventPayload(const std::string& sessionId) : sessionId(sessionId)
    {}

    std::string sessionId;
};

struct LogoutSocketEvent : public SocketEvent<LogoutSocketEventPayload>
{
    LogoutSocketEvent(LogoutSocketEventPayload payload) :  SocketEvent(SocketEventName::LOGOUT, payload)
    {}
};
