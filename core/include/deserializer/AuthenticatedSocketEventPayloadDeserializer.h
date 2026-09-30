#pragma once

#include "IDeserializer.h"
#include "socket/AuthenticatedSocketEvent.h"

class AuthenticatedSocketEventPayloadDeserializer: public IDeserializer<std::string, AuthenticatedSocketEventPayload>
{
public:
    AuthenticatedSocketEventPayloadDeserializer() = default;

    AuthenticatedSocketEventPayload Deserialize(const std::string& jsonString) override;
};
