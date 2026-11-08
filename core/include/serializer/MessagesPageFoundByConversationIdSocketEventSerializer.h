#pragma once

#include "ISerializer.h"
#include "socket/MessagesPageFoundByConversationIdSocketEvent.h"

class MessagesPageFoundByConversationIdSocketEventSerializer: public ISerializer<MessagesPageFoundByConversationIdSocketEvent, std::string>
{
public:
    MessagesPageFoundByConversationIdSocketEventSerializer() = default;

    std::string Serialize(const MessagesPageFoundByConversationIdSocketEvent& messagesPageFoundByConversationIdSocketEvent) override;
};
