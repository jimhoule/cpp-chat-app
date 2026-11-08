#pragma once

#include "IDeserializer.h"
#include "socket/FindMessagesPageByConversationIdSocketEvent.h"

class FindMessagesPageByConversationIdSocketEventPayloadDeserializer: public IDeserializer<std::string, FindMessagesPageByConversationIdSocketEventPayload>
{
public:
    FindMessagesPageByConversationIdSocketEventPayloadDeserializer() = default;

    FindMessagesPageByConversationIdSocketEventPayload Deserialize(const std::string& jsonString) override;
};
