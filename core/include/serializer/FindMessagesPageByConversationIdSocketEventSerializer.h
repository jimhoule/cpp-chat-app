#pragma once

#include "ISerializer.h"
#include "socket/FindMessagesPageByConversationIdSocketEvent.h"

class FindMessagesPageByConversationIdSocketEventSerializer: public ISerializer<FindMessagesPageByConversationIdSocketEvent, std::string>
{
public:
    FindMessagesPageByConversationIdSocketEventSerializer() = default;

    std::string Serialize(const FindMessagesPageByConversationIdSocketEvent& findMessagesPageByConversationIdSocketEvent) override;
};
