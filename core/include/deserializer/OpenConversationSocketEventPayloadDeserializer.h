#pragma once

#include "IDeserializer.h"
#include "socket/OpenConversationSocketEvent.h"

class OpenConversationSocketEventPayloadDeserializer: public IDeserializer<std::string, OpenConversationSocketEventPayload>
{
public:
    OpenConversationSocketEventPayloadDeserializer() = default;

    OpenConversationSocketEventPayload Deserialize(const std::string& jsonString) override;
};
