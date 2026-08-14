#pragma once

#include "models/Conversation.h"
#include "socket/SocketEvent.h"

#include <string>
#include <vector>

struct FindConversationByUserIdsSocketEventPayload
{
    FindConversationByUserIdsSocketEventPayload() = default;
    FindConversationByUserIdsSocketEventPayload(const std::vector<std::string>& userIds)
        : userIds(userIds)
    {}

    std::vector<std::string> userIds;
};

struct FindConversationByUserIdsSocketEvent : public SocketEvent<FindConversationByUserIdsSocketEventPayload>
{
    FindConversationByUserIdsSocketEvent(FindConversationByUserIdsSocketEventPayload payload) :  SocketEvent(SocketEventName::FIND_CONVERSATION_BY_USER_IDS, payload)
    {}
};