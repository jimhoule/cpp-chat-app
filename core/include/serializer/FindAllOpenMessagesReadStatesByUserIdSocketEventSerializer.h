#pragma once

#include "ISerializer.h"
#include "socket/FindAllOpenMessagesReadStatesByUserIdSocketEvent.h"

class FindAllOpenMessagesReadStatesByUserIdSocketEventSerializer: public ISerializer<FindAllOpenMessagesReadStatesByUserIdSocketEvent, std::string>
{
public:
    FindAllOpenMessagesReadStatesByUserIdSocketEventSerializer() = default;

    std::string Serialize(const FindAllOpenMessagesReadStatesByUserIdSocketEvent& findAllOpenMessagesReadStatesByUserIdSocketEvent) override;
};
