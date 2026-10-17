#pragma once

#include "views/ConversationView.h"

#include <vector>

struct AllOpenConversationsFoundEvent
{
    AllOpenConversationsFoundEvent() = default;
    AllOpenConversationsFoundEvent(const std::vector<ConversationView>& conversations)
        : conversations(conversations)
    {}

    std::vector<ConversationView> conversations = {};
};
