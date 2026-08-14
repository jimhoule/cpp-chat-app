#pragma once

#include "models/Conversation.h"
#include "socket/SocketEvent.h"

#include <vector>

struct AllOpenConversationsFoundByUserIdSocketEventPayload
{
    AllOpenConversationsFoundByUserIdSocketEventPayload() = default;
    AllOpenConversationsFoundByUserIdSocketEventPayload(const std::vector<Conversation>& conversations)
        : conversations(conversations)
    {}

    std::vector<Conversation> conversations;
};

struct AllOpenConversationsFoundByUserIdSocketEvent : public SocketEvent<AllOpenConversationsFoundByUserIdSocketEventPayload>
{
    AllOpenConversationsFoundByUserIdSocketEvent(AllOpenConversationsFoundByUserIdSocketEventPayload payload) :  SocketEvent(SocketEventName::ALL_OPEN_CONVERSATIONS_FOUND_BY_USER_ID, payload)
    {}
};