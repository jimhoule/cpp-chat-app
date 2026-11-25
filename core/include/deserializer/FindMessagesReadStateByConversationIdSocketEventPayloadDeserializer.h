#pragma once

#include "IDeserializer.h"
#include "socket/FindMessagesReadStateByConversationIdSocketEvent.h"

class FindMessagesReadStateByConversationIdSocketEventPayloadDeserializer: public IDeserializer<std::string, FindMessagesReadStateByConversationIdSocketEventPayload>
{
public:
    FindMessagesReadStateByConversationIdSocketEventPayloadDeserializer() = default;

    FindMessagesReadStateByConversationIdSocketEventPayload Deserialize(const std::string& jsonString) override;
};
