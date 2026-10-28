#pragma once

#include "deserializer/IDeserializer.h"
#include "socket/ConversationReopenedSocketEvent.h"

class ConversationReopenedSocketEventPayloadDeserializer: public IDeserializer<std::string, ConversationReopenedSocketEventPayload>
{
public:
    ConversationReopenedSocketEventPayloadDeserializer() = default;

    ConversationReopenedSocketEventPayload Deserialize(const std::string& jsonString) override;
};
