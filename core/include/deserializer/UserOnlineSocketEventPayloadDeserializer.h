#pragma once

#include "IDeserializer.h"
#include "socket/UserOnlineSocketEvent.h"

class UserOnlineSocketEventPayloadDeserializer: public IDeserializer<std::string, UserOnlineSocketEventPayload>
{
public:
    UserOnlineSocketEventPayloadDeserializer() = default;

    UserOnlineSocketEventPayload Deserialize(const std::string& jsonString) override;
};
