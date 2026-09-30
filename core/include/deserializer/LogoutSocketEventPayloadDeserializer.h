#pragma once

#include "IDeserializer.h"
#include "socket/LogoutSocketEvent.h"

class LogoutSocketEventPayloadDeserializer: public IDeserializer<std::string, LogoutSocketEventPayload>
{
public:
    LogoutSocketEventPayloadDeserializer() = default;

    LogoutSocketEventPayload Deserialize(const std::string& jsonString) override;
};
