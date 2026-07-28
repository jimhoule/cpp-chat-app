#pragma once

#include "socket/SocketEventName.h"

template<typename TPayload>
struct SocketEvent
{
public:
    SocketEvent(SocketEventName name, TPayload payload) : name(name), payload(payload)
    {}

    SocketEventName name;
    TPayload payload;
};
