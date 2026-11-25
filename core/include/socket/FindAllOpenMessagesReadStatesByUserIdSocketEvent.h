#pragma once

#include "socket/SocketEvent.h"

struct FindAllOpenMessagesReadStatesByUserIdSocketEventPayload
{
    FindAllOpenMessagesReadStatesByUserIdSocketEventPayload() = default;
};

struct FindAllOpenMessagesReadStatesByUserIdSocketEvent : public SocketEvent<FindAllOpenMessagesReadStatesByUserIdSocketEventPayload>
{
    FindAllOpenMessagesReadStatesByUserIdSocketEvent(FindAllOpenMessagesReadStatesByUserIdSocketEventPayload payload) :  SocketEvent(SocketEventName::FIND_ALL_OPEN_MESSAGES_READ_STATES_BY_USER_ID, payload)
    {}
};
