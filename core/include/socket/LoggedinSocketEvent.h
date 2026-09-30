#pragma once

#include "models/User.h"
#include "SocketEvent.h"

#include <string>

struct LoggedinSocketEventPayload
{
    LoggedinSocketEventPayload() = default;
    LoggedinSocketEventPayload(const std::string& sessionId, const User& user) : sessionId(sessionId), user(user)
    {}

    std::string sessionId;
    User user = {};
};

struct LoggedinSocketEvent : public SocketEvent<LoggedinSocketEventPayload>
{
    LoggedinSocketEvent(LoggedinSocketEventPayload payload) :  SocketEvent(SocketEventName::LOGGEDIN, payload)
    {}
};
