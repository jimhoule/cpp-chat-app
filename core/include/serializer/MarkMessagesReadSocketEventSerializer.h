#pragma once

#include "ISerializer.h"
#include "socket/MarkMessagesReadSocketEvent.h"

class MarkMessagesReadSocketEventSerializer: public ISerializer<MarkMessagesReadSocketEvent, std::string>
{
public:
    MarkMessagesReadSocketEventSerializer() = default;

    std::string Serialize(const MarkMessagesReadSocketEvent& markMessagesReadSocketEvent) override;
};
