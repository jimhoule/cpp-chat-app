#pragma once

#include "views/ConversationView.h"

struct ConversationOpenedEvent
{
    ConversationOpenedEvent() = default;
    ConversationOpenedEvent(const ConversationView& conversation)
        : conversation(conversation)
    {}

    ConversationView conversation = {};
};
