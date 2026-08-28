#pragma once

#include "models/Message.h"

struct MessageCreatedEvent
{
    MessageCreatedEvent() = default;
    MessageCreatedEvent(const Message& message)
        : message(message)
    {}

    Message message = {};
};
