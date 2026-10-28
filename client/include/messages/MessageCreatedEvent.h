#pragma once

#include "views/MessageView.h"

struct MessageCreatedEvent
{
    MessageCreatedEvent() = default;
    MessageCreatedEvent(const MessageView& message)
        : message(message)
    {}

    MessageView message = {};
};
