#pragma once

#include "IDeserializer.h"
#include "socket/MessageCreatedSocketEvent.h"

class MessageCreatedSocketEventPayloadDeserializer: public IDeserializer<std::string, MessageCreatedSocketEventPayload>
{
public:
    MessageCreatedSocketEventPayloadDeserializer() = default;

    MessageCreatedSocketEventPayload Deserialize(const std::string& jsonString) override;
};
