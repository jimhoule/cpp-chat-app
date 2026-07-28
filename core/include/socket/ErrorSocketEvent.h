#pragma once

#include "socket/SocketEvent.h"
#include "socket/SocketErrorCode.h"

struct ErrorSocketEventPayload
{
    ErrorSocketEventPayload() = default;
    ErrorSocketEventPayload(SocketErrorCode code)
        : code(code)
    {}

    // NOTE: Defaults to INTERNAL so a payload that is never filled in leaks nothing about what happened
    SocketErrorCode code = SocketErrorCode::INTERNAL;
};

struct ErrorSocketEvent : public SocketEvent<ErrorSocketEventPayload>
{
    ErrorSocketEvent(ErrorSocketEventPayload payload)
        : SocketEvent(SocketEventName::ERROR, payload)
    {}
};
