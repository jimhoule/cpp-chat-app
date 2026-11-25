#pragma once

#include "serializer/ISerializer.h"
#include "socket/AllOpenMessagesReadStatesFoundByUserIdSocketEvent.h"

class AllOpenMessagesReadStatesFoundByUserIdSocketEventSerializer: public ISerializer<AllOpenMessagesReadStatesFoundByUserIdSocketEvent, std::string>
{
public:
    AllOpenMessagesReadStatesFoundByUserIdSocketEventSerializer() = default;

    std::string Serialize(const AllOpenMessagesReadStatesFoundByUserIdSocketEvent& allOpenMessagesReadStatesFoundByUserIdSocketEvent) override;
};
