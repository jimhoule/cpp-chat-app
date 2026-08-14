#pragma once

#include "models/Conversation.h"
#include "socket/SocketEvent.h"

#include <optional>
#include <vector>

struct ConversationOpenedSocketEventPayload
{
    ConversationOpenedSocketEventPayload() = default;
    ConversationOpenedSocketEventPayload(const Conversation& conversation)
        : conversation(conversation)
    {}

    Conversation conversation;
};

struct ConversationOpenedSocketEvent : public SocketEvent<ConversationOpenedSocketEventPayload>
{
    ConversationOpenedSocketEvent(ConversationOpenedSocketEventPayload payload) :  SocketEvent(SocketEventName::CONVERSATION_OPENED, payload)
    {}
};