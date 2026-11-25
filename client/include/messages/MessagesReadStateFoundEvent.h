#pragma once

#include "views/MessagesReadStateView.h"

struct MessagesReadStateFoundEvent
{
    MessagesReadStateFoundEvent() = default;
    MessagesReadStateFoundEvent(const MessagesReadStateView& messagesReadState)
        : messagesReadState(messagesReadState)
    {}

    MessagesReadStateView messagesReadState = {};
};
