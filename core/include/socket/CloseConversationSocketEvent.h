#pragma once

#include "models/Conversation.h"
#include "socket/SocketEvent.h"

#include <string>

struct CloseConversationSocketEventPayload
{
    CloseConversationSocketEventPayload() = default;
    CloseConversationSocketEventPayload(const std::string& conversationId)
        : conversationId(conversationId)
    {}

    std::string conversationId;
};

struct CloseConversationSocketEvent : public SocketEvent<CloseConversationSocketEventPayload>
{
    CloseConversationSocketEvent(CloseConversationSocketEventPayload payload) :  SocketEvent(SocketEventName::CLOSE_CONVERSATION, payload)
    {}
};