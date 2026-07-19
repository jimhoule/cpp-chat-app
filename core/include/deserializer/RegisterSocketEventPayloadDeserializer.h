#pragma once

#include "IDeserializer.h"
#include "socket/RegisterSocketEvent.h"

class RegisterSocketEventPayloadDeserializer: public IDeserializer<std::string, RegisterSocketEventPayload>
{
public:
    RegisterSocketEventPayloadDeserializer() = default;

    RegisterSocketEventPayload Deserialize(const std::string& jsonString) override;
};
