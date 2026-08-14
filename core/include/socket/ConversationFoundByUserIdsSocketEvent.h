#pragma once

#include "models/Conversation.h"
#include "socket/SocketEvent.h"

#include <optional>
#include <vector>

struct ConversationFoundByUserIdsSocketEventPayload
{
    ConversationFoundByUserIdsSocketEventPayload() = default;
    ConversationFoundByUserIdsSocketEventPayload(const std::optional<Conversation>& conversation)
        : conversation(conversation)
    {}

    std::optional<Conversation> conversation;
};

struct ConversationFoundByUserIdsSocketEvent : public SocketEvent<ConversationFoundByUserIdsSocketEventPayload>
{
    ConversationFoundByUserIdsSocketEvent(ConversationFoundByUserIdsSocketEventPayload payload) :  SocketEvent(SocketEventName::CONVERSATION_FOUND_BY_USER_IDS, payload)
    {}
};