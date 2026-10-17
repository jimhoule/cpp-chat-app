#pragma once

#include "socket/SocketEvent.h"
#include "views/ConversationView.h"

struct ConversationClosedSocketEventPayload
{
    ConversationClosedSocketEventPayload() = default;
    ConversationClosedSocketEventPayload(const ConversationView& conversation)
        : conversation(conversation)
    {}

    ConversationView conversation;
};

struct ConversationClosedSocketEvent : public SocketEvent<ConversationClosedSocketEventPayload>
{
    ConversationClosedSocketEvent(ConversationClosedSocketEventPayload payload) :  SocketEvent(SocketEventName::CONVERSATION_CLOSED, payload)
    {}
};