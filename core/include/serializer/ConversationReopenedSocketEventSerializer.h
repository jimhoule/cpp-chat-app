#pragma once

#include "ISerializer.h"
#include "socket/ConversationReopenedSocketEvent.h"

class ConversationReopenedSocketEventSerializer: public ISerializer<ConversationReopenedSocketEvent, std::string>
{
public:
    ConversationReopenedSocketEventSerializer() = default;

    std::string Serialize(const ConversationReopenedSocketEvent& conversationReopenedSocketEvent) override;
};
