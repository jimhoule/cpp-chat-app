#pragma once

#include "socket/SocketEvent.h"
#include "views/MessagesPageView.h"

#include <string>

struct MessagesPageFoundByConversationIdSocketEventPayload
{
    MessagesPageFoundByConversationIdSocketEventPayload() = default;
    MessagesPageFoundByConversationIdSocketEventPayload(const MessagesPageView& messagesPage, const std::string& conversationId)
        : messagesPage(messagesPage)
        , conversationId(conversationId)
    {}

    MessagesPageView messagesPage;
    std::string conversationId;
};

struct MessagesPageFoundByConversationIdSocketEvent : public SocketEvent<MessagesPageFoundByConversationIdSocketEventPayload>
{
    MessagesPageFoundByConversationIdSocketEvent(MessagesPageFoundByConversationIdSocketEventPayload payload) :  SocketEvent(SocketEventName::MESSAGES_PAGE_FOUND_BY_CONVERSATION_ID, payload)
    {}
};
