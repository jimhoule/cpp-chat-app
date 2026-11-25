#pragma once

#include "IDeserializer.h"
#include "socket/MarkMessagesReadSocketEvent.h"

class MarkMessagesReadSocketEventPayloadDeserializer: public IDeserializer<std::string, MarkMessagesReadSocketEventPayload>
{
public:
    MarkMessagesReadSocketEventPayloadDeserializer() = default;

    MarkMessagesReadSocketEventPayload Deserialize(const std::string& jsonString) override;
};
