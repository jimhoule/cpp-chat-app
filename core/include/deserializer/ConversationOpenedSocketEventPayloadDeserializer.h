#pragma once

#include "deserializer/IDeserializer.h"
#include "socket/ConversationOpenedSocketEvent.h"

class ConversationOpenedSocketEventPayloadDeserializer: public IDeserializer<std::string, ConversationOpenedSocketEventPayload>
{
public:
    ConversationOpenedSocketEventPayloadDeserializer() = default;

    ConversationOpenedSocketEventPayload Deserialize(const std::string& jsonString) override;
};
