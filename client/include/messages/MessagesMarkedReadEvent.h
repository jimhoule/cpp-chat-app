#pragma once

#include "views/MessagesReadStateView.h"

struct MessagesMarkedReadEvent
{
    MessagesMarkedReadEvent() = default;
    MessagesMarkedReadEvent(const MessagesReadStateView& messagesReadState)
        : messagesReadState(messagesReadState)
    {}

    MessagesReadStateView messagesReadState = {};
};
