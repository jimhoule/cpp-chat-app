#pragma once

#include "socket/SocketEvent.h"
#include "views/ConversationView.h"

/**
 * NOTES:
 *  - Sent when someone's message reopens a conversation a user had closed
 *  - Not the answer to the user's own OPEN_CONVERSATION (that is CONVERSATION_OPENED)
 */
struct ConversationReopenedSocketEventPayload
{
    ConversationReopenedSocketEventPayload() = default;
    ConversationReopenedSocketEventPayload(const ConversationView& conversation)
        : conversation(conversation)
    {}

    ConversationView conversation;
};

struct ConversationReopenedSocketEvent : public SocketEvent<ConversationReopenedSocketEventPayload>
{
    ConversationReopenedSocketEvent(ConversationReopenedSocketEventPayload payload) :  SocketEvent(SocketEventName::CONVERSATION_REOPENED, payload)
    {}
};