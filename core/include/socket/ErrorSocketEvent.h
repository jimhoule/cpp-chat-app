#pragma once

#include "socket/SocketErrorCode.h"
#include "socket/SocketEvent.h"
#include "socket/SocketEventName.h"

struct ErrorSocketEventPayload
{
    ErrorSocketEventPayload() = default;
    ErrorSocketEventPayload(SocketErrorCode code, SocketEventName failedEventName)
        : code(code)
        , failedEventName(failedEventName)
    {}

    // NOTE: Defaults to INTERNAL so a payload that is never filled in leaks nothing about what happened
    SocketErrorCode code = SocketErrorCode::INTERNAL;
    // NOTE: Defaults to ERROR because no clients sens ERROR so a payload that is never filled in matches no API's filter
    SocketEventName failedEventName = SocketEventName::ERROR;
};

struct ErrorSocketEvent : public SocketEvent<ErrorSocketEventPayload>
{
    ErrorSocketEvent(ErrorSocketEventPayload payload)
        : SocketEvent(SocketEventName::ERROR, payload)
    {}
};
