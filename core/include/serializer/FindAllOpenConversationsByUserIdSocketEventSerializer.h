#pragma once

#include "ISerializer.h"
#include "socket/FindAllOpenConversationsByUserIdSocketEvent.h"

class FindAllOpenConversationsByUserIdSocketEventSerializer: public ISerializer<FindAllOpenConversationsByUserIdSocketEvent, std::string>
{
public:
    FindAllOpenConversationsByUserIdSocketEventSerializer() = default;

    std::string Serialize(const FindAllOpenConversationsByUserIdSocketEvent& findAllOpenConversationsByUserIdSocketEvent) override;
};
