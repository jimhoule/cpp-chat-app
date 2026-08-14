#pragma once

#include "ISerializer.h"
#include "socket/ConversationClosedSocketEvent.h"

class ConversationClosedSocketEventSerializer: public ISerializer<ConversationClosedSocketEvent, std::string>
{
public:
    ConversationClosedSocketEventSerializer() = default;

    std::string Serialize(const ConversationClosedSocketEvent& conversationClosedSocketEvent) override;
};
