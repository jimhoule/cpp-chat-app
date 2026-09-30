#pragma once

#include "IDeserializer.h"
#include "socket/AuthenticateSocketEvent.h"

class AuthenticateSocketEventPayloadDeserializer: public IDeserializer<std::string, AuthenticateSocketEventPayload>
{
public:
    AuthenticateSocketEventPayloadDeserializer() = default;

    AuthenticateSocketEventPayload Deserialize(const std::string& jsonString) override;
};
