#pragma once

#include "deserializer/IDeserializer.h"
#include "socket/AllOpenMessagesReadStatesFoundByUserIdSocketEvent.h"

class AllOpenMessagesReadStatesFoundByUserIdSocketEventPayloadDeserializer: public IDeserializer<std::string, AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload>
{
public:
    AllOpenMessagesReadStatesFoundByUserIdSocketEventPayloadDeserializer() = default;

    AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload Deserialize(const std::string& jsonString) override;
};
