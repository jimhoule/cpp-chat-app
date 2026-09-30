#pragma once

#include "IDeserializer.h"
#include "socket/UserOfflineSocketEvent.h"

class UserOfflineSocketEventPayloadDeserializer: public IDeserializer<std::string, UserOfflineSocketEventPayload>
{
public:
    UserOfflineSocketEventPayloadDeserializer() = default;

    UserOfflineSocketEventPayload Deserialize(const std::string& jsonString) override;
};
