#pragma once

#include "models/Conversation.h"
#include "socket/SocketEvent.h"

#include <string>
#include <vector>

struct OpenConversationSocketEventPayload
{
    OpenConversationSocketEventPayload() = default;
    OpenConversationSocketEventPayload(const std::vector<std::string>& userIds)
        : userIds(userIds)
    {}

    std::vector<std::string> userIds;
};

struct OpenConversationSocketEvent : public SocketEvent<OpenConversationSocketEventPayload>
{
    OpenConversationSocketEvent(OpenConversationSocketEventPayload payload) :  SocketEvent(SocketEventName::OPEN_CONVERSATION, payload)
    {}
};