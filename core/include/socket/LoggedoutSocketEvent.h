#pragma once

#include "socket/SocketEvent.h"

#include <string>

struct LoggedoutSocketEventPayload
{
    LoggedoutSocketEventPayload() = default;
};

struct LoggedoutSocketEvent : public SocketEvent<LoggedoutSocketEventPayload>
{
    LoggedoutSocketEvent(LoggedoutSocketEventPayload payload) :  SocketEvent(SocketEventName::LOGGEDOUT, payload)
    {}
};
