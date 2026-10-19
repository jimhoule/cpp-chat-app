#pragma once

#include "deserializer/IDeserializer.h"
#include "socket/ConversationClosedSocketEvent.h"

class ConversationClosedSocketEventPayloadDeserializer: public IDeserializer<std::string, ConversationClosedSocketEventPayload>
{
public:
    ConversationClosedSocketEventPayloadDeserializer() = default;

    ConversationClosedSocketEventPayload Deserialize(const std::string& jsonString) override;
};
