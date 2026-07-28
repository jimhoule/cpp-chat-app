#pragma once

#include "IDeserializer.h"
#include "socket/LoggedinSocketEvent.h"

class LoggedinSocketEventPayloadDeserializer: public IDeserializer<std::string, LoggedinSocketEventPayload>
{
public:
    LoggedinSocketEventPayloadDeserializer() = default;

    LoggedinSocketEventPayload Deserialize(const std::string& jsonString) override;
};
