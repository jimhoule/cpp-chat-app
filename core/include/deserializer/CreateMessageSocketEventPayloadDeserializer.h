#pragma once

#include "IDeserializer.h"
#include "socket/CreateMessageSocketEvent.h"

class CreateMessageSocketEventPayloadDeserializer: public IDeserializer<std::string, CreateMessageSocketEventPayload>
{
public:
    CreateMessageSocketEventPayloadDeserializer() = default;

    CreateMessageSocketEventPayload Deserialize(const std::string& jsonString) override;
};
