#pragma once

#include "IDeserializer.h"
#include "socket/RegisteredSocketEvent.h"

class RegisteredSocketEventPayloadDeserializer: public IDeserializer<std::string, RegisteredSocketEventPayload>
{
public:
    RegisteredSocketEventPayloadDeserializer() = default;

    RegisteredSocketEventPayload Deserialize(const std::string& jsonString) override;
};
