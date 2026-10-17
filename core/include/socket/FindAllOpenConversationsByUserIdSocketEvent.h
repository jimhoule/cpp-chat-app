#pragma once

#include "socket/SocketEvent.h"

struct FindAllOpenConversationsByUserIdSocketEventPayload
{
    FindAllOpenConversationsByUserIdSocketEventPayload() = default;
};

struct FindAllOpenConversationsByUserIdSocketEvent : public SocketEvent<FindAllOpenConversationsByUserIdSocketEventPayload>
{
    FindAllOpenConversationsByUserIdSocketEvent(FindAllOpenConversationsByUserIdSocketEventPayload payload) :  SocketEvent(SocketEventName::FIND_ALL_OPEN_CONVERSATIONS_BY_USER_ID, payload)
    {}
};
