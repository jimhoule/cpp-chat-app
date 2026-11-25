#pragma once

#include "ISerializer.h"
#include "socket/FindMessagesReadStateByConversationIdSocketEvent.h"

class FindMessagesReadStateByConversationIdSocketEventSerializer: public ISerializer<FindMessagesReadStateByConversationIdSocketEvent, std::string>
{
public:
    FindMessagesReadStateByConversationIdSocketEventSerializer() = default;

    std::string Serialize(const FindMessagesReadStateByConversationIdSocketEvent& findMessagesReadStateByConversationIdSocketEvent) override;
};
