#pragma once

#include "socket/SocketEvent.h"
#include "views/ConversationView.h"

struct ConversationOpenedSocketEventPayload
{
    ConversationOpenedSocketEventPayload() = default;
    ConversationOpenedSocketEventPayload(const ConversationView& conversation)
        : conversation(conversation)
    {}

    ConversationView conversation;
};

struct ConversationOpenedSocketEvent : public SocketEvent<ConversationOpenedSocketEventPayload>
{
    ConversationOpenedSocketEvent(ConversationOpenedSocketEventPayload payload) :  SocketEvent(SocketEventName::CONVERSATION_OPENED, payload)
    {}
};