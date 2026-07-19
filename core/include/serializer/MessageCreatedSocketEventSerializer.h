#pragma once

#include "ISerializer.h"
#include "socket/MessageCreatedSocketEvent.h"

class MessageCreatedSocketEventSerializer: public ISerializer<MessageCreatedSocketEvent, std::string>
{
public:
    MessageCreatedSocketEventSerializer() = default;

    std::string Serialize(const MessageCreatedSocketEvent& messageCreatedSocketEvent) override;
};
