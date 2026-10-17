#pragma once

#include "socket/SocketEvent.h"

struct LoggedoutSocketEventPayload
{
    LoggedoutSocketEventPayload() = default;
};

struct LoggedoutSocketEvent : public SocketEvent<LoggedoutSocketEventPayload>
{
    LoggedoutSocketEvent(LoggedoutSocketEventPayload payload) :  SocketEvent(SocketEventName::LOGGEDOUT, payload)
    {}
};
