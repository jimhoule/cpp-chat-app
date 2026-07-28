#pragma once

#include "SocketEvent.h"

#include <string>

struct LoggedinSocketEventPayload
{
    LoggedinSocketEventPayload() = default;
    LoggedinSocketEventPayload(const std::string& sessionId) : sessionId(sessionId)
    {}

    std::string sessionId;
};

struct LoggedinSocketEvent : public SocketEvent<LoggedinSocketEventPayload>
{
    LoggedinSocketEvent(LoggedinSocketEventPayload payload) :  SocketEvent(SocketEventName::LOGGEDIN, payload)
    {}
};
