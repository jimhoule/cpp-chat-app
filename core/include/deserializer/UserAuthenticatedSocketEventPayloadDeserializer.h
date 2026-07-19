#pragma once

#include "IDeserializer.h"
#include "socket/UserAuthenticatedSocketEvent.h"

class UserAuthenticatedSocketEventPayloadDeserializer: public IDeserializer<std::string, UserAuthenticatedSocketEventPayload>
{
public:
    UserAuthenticatedSocketEventPayloadDeserializer() = default;

    UserAuthenticatedSocketEventPayload Deserialize(const std::string& jsonString) override;
};
