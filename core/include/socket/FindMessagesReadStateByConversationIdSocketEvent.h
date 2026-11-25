#pragma once

#include "socket/SocketEvent.h"

#include <string>

struct FindMessagesReadStateByConversationIdSocketEventPayload
{
    FindMessagesReadStateByConversationIdSocketEventPayload() = default;
    FindMessagesReadStateByConversationIdSocketEventPayload(const std::string& conversationId)
        : conversationId(conversationId)
    {}

    std::string conversationId;
};

struct FindMessagesReadStateByConversationIdSocketEvent : public SocketEvent<FindMessagesReadStateByConversationIdSocketEventPayload>
{
    FindMessagesReadStateByConversationIdSocketEvent(FindMessagesReadStateByConversationIdSocketEventPayload payload) :  SocketEvent(SocketEventName::FIND_MESSAGES_READ_STATE_BY_CONVERSATION_ID, payload)
    {}
};
