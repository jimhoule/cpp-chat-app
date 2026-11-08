#pragma once

#include "socket/SocketEvent.h"

#include <string>

struct FindMessagesPageByConversationIdSocketEventPayload
{
    FindMessagesPageByConversationIdSocketEventPayload() = default;
    FindMessagesPageByConversationIdSocketEventPayload(const std::string& conversationId, const std::string& cursor)
        : conversationId(conversationId)
        , cursor(cursor)
    {}

    std::string conversationId;
    // NOTE: Empty on the first page (otherwise it is the next cursor of the previous page)
    std::string cursor;
};

struct FindMessagesPageByConversationIdSocketEvent : public SocketEvent<FindMessagesPageByConversationIdSocketEventPayload>
{
    FindMessagesPageByConversationIdSocketEvent(FindMessagesPageByConversationIdSocketEventPayload payload) :  SocketEvent(SocketEventName::FIND_MESSAGES_PAGE_BY_CONVERSATION_ID, payload)
    {}
};
