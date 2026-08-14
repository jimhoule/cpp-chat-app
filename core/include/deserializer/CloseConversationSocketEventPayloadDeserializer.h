#pragma once

#include "IDeserializer.h"
#include "socket/CloseConversationSocketEvent.h"

class CloseConversationSocketEventPayloadDeserializer: public IDeserializer<std::string, CloseConversationSocketEventPayload>
{
public:
    CloseConversationSocketEventPayloadDeserializer() = default;

    CloseConversationSocketEventPayload Deserialize(const std::string& jsonString) override;
};
