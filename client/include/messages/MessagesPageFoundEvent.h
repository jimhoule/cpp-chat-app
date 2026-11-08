#pragma once

#include "views/MessagesPageView.h"

#include <string>

struct MessagesPageFoundEvent
{
    MessagesPageFoundEvent() = default;
    MessagesPageFoundEvent(const MessagesPageView& messagesPage, const std::string& conversationId)
        : messagesPage(messagesPage)
        , conversationId(conversationId)
    {}

    MessagesPageView messagesPage = {};
    std::string conversationId;
};
