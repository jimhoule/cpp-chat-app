#pragma once

#include "deserializer/IDeserializer.h"
#include "socket/MessagesMarkedReadSocketEvent.h"

class MessagesMarkedReadSocketEventPayloadDeserializer: public IDeserializer<std::string, MessagesMarkedReadSocketEventPayload>
{
public:
    MessagesMarkedReadSocketEventPayloadDeserializer() = default;

    MessagesMarkedReadSocketEventPayload Deserialize(const std::string& jsonString) override;
};
