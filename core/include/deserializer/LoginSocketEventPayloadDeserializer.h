#pragma once

#include "IDeserializer.h"
#include "socket/LoginSocketEvent.h"

class LoginSocketEventPayloadDeserializer: public IDeserializer<std::string, LoginSocketEventPayload>
{
public:
    LoginSocketEventPayloadDeserializer() = default;

    LoginSocketEventPayload Deserialize(const std::string& JsonString) override;
};
