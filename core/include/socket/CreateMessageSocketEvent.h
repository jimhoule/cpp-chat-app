#pragma once

#include "SocketEvent.h"

#include <string>

struct CreateMessageSocketEventPayload
{
    CreateMessageSocketEventPayload() = default;
    CreateMessageSocketEventPayload(const std::string& conversationID, const std::string& text) : conversationID(conversationID), text(text)
    {}

    std::string conversationID;
    std::string text;
};

struct CreateMessageSocketEvent : public SocketEvent<CreateMessageSocketEventPayload>
{
    CreateMessageSocketEvent(CreateMessageSocketEventPayload Payload) :  SocketEvent(SocketEventName::CREATE_MESSAGE, Payload)
    {}
};
