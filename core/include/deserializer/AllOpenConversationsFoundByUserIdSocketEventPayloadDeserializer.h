#pragma once

#include "deserializer/IDeserializer.h"
#include "socket/AllOpenConversationsFoundByUserIdSocketEvent.h"

class AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer: public IDeserializer<std::string, AllOpenConversationsFoundByUserIdSocketEventPayload>
{
public:
    AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer() = default;

    AllOpenConversationsFoundByUserIdSocketEventPayload Deserialize(const std::string& jsonString) override;
};
