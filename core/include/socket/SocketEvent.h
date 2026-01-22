#pragma once

#include "socket/SocketEventName.h"

template<typename TPayload>
struct SocketEvent
{
public:
    SocketEvent() = default;
    SocketEvent(SocketEventName Name, TPayload Payload) : Name(Name), Payload(Payload)
    {}

    SocketEventName Name;
    TPayload Payload;
};
