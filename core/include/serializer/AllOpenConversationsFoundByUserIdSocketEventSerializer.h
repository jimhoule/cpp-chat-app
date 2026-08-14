#pragma once

#include "ISerializer.h"
#include "socket/AllOpenConversationsFoundByUserIdSocketEvent.h"

class AllOpenConversationsFoundByUserIdSocketEventSerializer: public ISerializer<AllOpenConversationsFoundByUserIdSocketEvent, std::string>
{
public:
    AllOpenConversationsFoundByUserIdSocketEventSerializer() = default;

    std::string Serialize(const AllOpenConversationsFoundByUserIdSocketEvent& allOpenConversationsFoundByUserIdSocketEvent) override;
};
