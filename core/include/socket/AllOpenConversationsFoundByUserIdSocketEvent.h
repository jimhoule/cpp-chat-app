#pragma once

#include "socket/SocketEvent.h"
#include "views/ConversationView.h"

#include <vector>

struct AllOpenConversationsFoundByUserIdSocketEventPayload
{
    AllOpenConversationsFoundByUserIdSocketEventPayload() = default;
    AllOpenConversationsFoundByUserIdSocketEventPayload(const std::vector<ConversationView>& conversations)
        : conversations(conversations)
    {}

    std::vector<ConversationView> conversations;
};

struct AllOpenConversationsFoundByUserIdSocketEvent : public SocketEvent<AllOpenConversationsFoundByUserIdSocketEventPayload>
{
    AllOpenConversationsFoundByUserIdSocketEvent(AllOpenConversationsFoundByUserIdSocketEventPayload payload) :  SocketEvent(SocketEventName::ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID, payload)
    {}
};