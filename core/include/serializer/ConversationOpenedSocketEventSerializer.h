#pragma once

#include "ISerializer.h"
#include "socket/ConversationOpenedSocketEvent.h"

class ConversationOpenedSocketEventSerializer: public ISerializer<ConversationOpenedSocketEvent, std::string>
{
public:
    ConversationOpenedSocketEventSerializer() = default;

    std::string Serialize(const ConversationOpenedSocketEvent& conversationOpenedSocketEvent) override;
};
