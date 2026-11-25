#pragma once

#include "socket/SocketEvent.h"

#include <string>

struct MarkMessagesReadSocketEventPayload
{
    MarkMessagesReadSocketEventPayload() = default;
    MarkMessagesReadSocketEventPayload(const std::string& conversationId, const std::string& messageId)
        : conversationId(conversationId)
        , messageId(messageId)
    {}

    std::string conversationId;
    // NOTE: The newest message id on the client's screen (every message up to this one is marked read)
    std::string messageId;
};

struct MarkMessagesReadSocketEvent : public SocketEvent<MarkMessagesReadSocketEventPayload>
{
    MarkMessagesReadSocketEvent(MarkMessagesReadSocketEventPayload payload) :  SocketEvent(SocketEventName::MARK_MESSAGES_READ, payload)
    {}
};
