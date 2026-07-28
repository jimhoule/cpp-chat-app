#pragma once

#include "SocketEvent.h"

#include <string>

struct CreateMessageSocketEventPayload
{
    CreateMessageSocketEventPayload() = default;
    CreateMessageSocketEventPayload(const std::string& conversationId, const std::string& text) : conversationId(conversationId), text(text)
    {}

    std::string conversationId;
    std::string text;
};

struct CreateMessageSocketEvent : public SocketEvent<CreateMessageSocketEventPayload>
{
    CreateMessageSocketEvent(CreateMessageSocketEventPayload payload) :  SocketEvent(SocketEventName::CREATE_MESSAGE, payload)
    {}
};
