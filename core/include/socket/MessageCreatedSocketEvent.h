#pragma once

#include "socket/SocketEvent.h"
#include "views/MessageView.h"

#include <string>

struct MessageCreatedSocketEventPayload
{
    MessageCreatedSocketEventPayload() = default;
    MessageCreatedSocketEventPayload(const MessageView& message) : message(message)
    {}

    MessageView message = {};
};

struct MessageCreatedSocketEvent : public SocketEvent<MessageCreatedSocketEventPayload>
{
    MessageCreatedSocketEvent(MessageCreatedSocketEventPayload payload) :  SocketEvent(SocketEventName::MESSAGE_CREATED, payload)
    {}
};
