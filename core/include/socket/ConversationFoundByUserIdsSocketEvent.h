#pragma once

#include "socket/SocketEvent.h"
#include "views/ConversationView.h"

#include <optional>
#include <vector>

struct ConversationFoundByUserIdsSocketEventPayload
{
    ConversationFoundByUserIdsSocketEventPayload() = default;
    ConversationFoundByUserIdsSocketEventPayload(const std::optional<ConversationView>& conversation)
        : conversation(conversation)
    {}

    std::optional<ConversationView> conversation;
};

struct ConversationFoundByUserIdsSocketEvent : public SocketEvent<ConversationFoundByUserIdsSocketEventPayload>
{
    ConversationFoundByUserIdsSocketEvent(ConversationFoundByUserIdsSocketEventPayload payload) :  SocketEvent(SocketEventName::CONVERSATION_FOUND_BY_USER_IDS, payload)
    {}
};