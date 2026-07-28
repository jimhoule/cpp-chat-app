#pragma once

#include "IDeserializer.h"
#include "socket/ErrorSocketEvent.h"

class ErrorSocketEventPayloadDeserializer: public IDeserializer<std::string, ErrorSocketEventPayload>
{
public:
    ErrorSocketEventPayloadDeserializer() = default;

    ErrorSocketEventPayload Deserialize(const std::string& jsonString) override;
};
