#pragma once

#include "ISerializer.h"
#include "socket/MessagesMarkedReadSocketEvent.h"

class MessagesMarkedReadSocketEventSerializer: public ISerializer<MessagesMarkedReadSocketEvent, std::string>
{
public:
    MessagesMarkedReadSocketEventSerializer() = default;

    std::string Serialize(const MessagesMarkedReadSocketEvent& messagesMarkedReadSocketEvent) override;
};
