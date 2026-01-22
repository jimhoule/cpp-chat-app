#pragma once

#include "IDeserializer.h"
#include "socket/SocketEvent.h"

class SocketEventDeserializer: public IDeserializer<std::string, SocketEvent<std::string>>
{
public:
    SocketEventDeserializer() = default;

    SocketEvent<std::string> Deserialize(const std::string& JsonString) override;
};
