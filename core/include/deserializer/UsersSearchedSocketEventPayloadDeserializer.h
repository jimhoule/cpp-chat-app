#pragma once

#include "deserializer/IDeserializer.h"
#include "socket/UsersSearchedSocketEvent.h"

class UsersSearchedSocketEventPayloadDeserializer: public IDeserializer<std::string, UsersSearchedSocketEventPayload>
{
public:
    UsersSearchedSocketEventPayloadDeserializer() = default;

    UsersSearchedSocketEventPayload Deserialize(const std::string& jsonString) override;
};
