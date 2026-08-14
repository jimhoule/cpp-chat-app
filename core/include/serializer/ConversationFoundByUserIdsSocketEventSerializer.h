#pragma once

#include "ISerializer.h"
#include "socket/ConversationFoundByUserIdsSocketEvent.h"

class ConversationFoundByUserIdsSocketEventSerializer: public ISerializer<ConversationFoundByUserIdsSocketEvent, std::string>
{
public:
    ConversationFoundByUserIdsSocketEventSerializer() = default;

    std::string Serialize(const ConversationFoundByUserIdsSocketEvent& conversationFoundByUserIdsSocketEvent) override;
};
