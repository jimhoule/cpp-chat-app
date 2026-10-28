#pragma once

#include "views/ConversationView.h"

struct ConversationReopenedEvent
{
    ConversationReopenedEvent() = default;
    ConversationReopenedEvent(const ConversationView& conversation)
        : conversation(conversation)
    {}

    ConversationView conversation = {};
};
