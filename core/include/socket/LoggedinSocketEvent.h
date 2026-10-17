#pragma once

#include "socket/SocketEvent.h"
#include "views/UserView.h"

#include <string>

struct LoggedinSocketEventPayload
{
    LoggedinSocketEventPayload() = default;
    LoggedinSocketEventPayload(const std::string& sessionId, const UserView& user) : sessionId(sessionId), user(user)
    {}

    std::string sessionId;
    UserView user = {};
};

struct LoggedinSocketEvent : public SocketEvent<LoggedinSocketEventPayload>
{
    LoggedinSocketEvent(LoggedinSocketEventPayload payload) :  SocketEvent(SocketEventName::LOGGEDIN, payload)
    {}
};
