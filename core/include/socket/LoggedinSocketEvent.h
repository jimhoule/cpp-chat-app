#pragma once

#include "SocketEvent.h"

#include <string>

struct LoggedinSocketEventPayload
{
    LoggedinSocketEventPayload() = default;
    LoggedinSocketEventPayload(const std::string& AccessToken) : AccessToken(AccessToken)
    {}

    std::string AccessToken;
};

struct LoggedinSocketEvent : public SocketEvent<LoggedinSocketEventPayload>
{
    LoggedinSocketEvent(LoggedinSocketEventPayload Payload) :  SocketEvent(SocketEventName::LOGGEDIN, Payload)
    {}
};
