#pragma once

#include "views/ConversationView.h"

struct ConversationClosedEvent
{
    ConversationClosedEvent() = default;
    ConversationClosedEvent(const ConversationView& conversation)
        : conversation(conversation)
    {}

    ConversationView conversation = {};
};
