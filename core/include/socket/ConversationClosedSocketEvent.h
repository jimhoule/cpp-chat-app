#pragma once

#include "models/Conversation.h"
#include "socket/SocketEvent.h"

#include <optional>
#include <vector>

struct ConversationClosedSocketEventPayload
{
    ConversationClosedSocketEventPayload() = default;
    ConversationClosedSocketEventPayload(const Conversation& conversation)
        : conversation(conversation)
    {}

    Conversation conversation;
};

struct ConversationClosedSocketEvent : public SocketEvent<ConversationClosedSocketEventPayload>
{
    ConversationClosedSocketEvent(ConversationClosedSocketEventPayload payload) :  SocketEvent(SocketEventName::CONVERSATION_CLOSED, payload)
    {}
};