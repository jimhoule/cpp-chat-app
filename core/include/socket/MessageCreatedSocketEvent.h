#pragma once

#include "models/Message.h"
#include "socket/SocketEvent.h"

#include <string>

struct MessageCreatedSocketEventPayload
{
    MessageCreatedSocketEventPayload() = default;
    MessageCreatedSocketEventPayload(const Message& message) : message(message)
    {}

    Message message = {};
};

struct MessageCreatedSocketEvent : public SocketEvent<MessageCreatedSocketEventPayload>
{
    MessageCreatedSocketEvent(MessageCreatedSocketEventPayload payload) :  SocketEvent(SocketEventName::MESSAGE_CREATED, payload)
    {}
};
