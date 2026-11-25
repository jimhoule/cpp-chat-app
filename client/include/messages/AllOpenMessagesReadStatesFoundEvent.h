#pragma once

#include "views/MessagesReadStateView.h"

#include <vector>

struct AllOpenMessagesReadStatesFoundEvent
{
    AllOpenMessagesReadStatesFoundEvent() = default;
    AllOpenMessagesReadStatesFoundEvent(const std::vector<MessagesReadStateView>& messagesReadStates)
        : messagesReadStates(messagesReadStates)
    {}

    std::vector<MessagesReadStateView> messagesReadStates = {};
};
