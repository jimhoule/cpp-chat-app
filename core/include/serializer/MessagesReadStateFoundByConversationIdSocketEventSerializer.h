#pragma once

#include "serializer/ISerializer.h"
#include "socket/MessagesReadStateFoundByConversationIdSocketEvent.h"

class MessagesReadStateFoundByConversationIdSocketEventSerializer: public ISerializer<MessagesReadStateFoundByConversationIdSocketEvent, std::string>
{
public:
    MessagesReadStateFoundByConversationIdSocketEventSerializer() = default;

    std::string Serialize(const MessagesReadStateFoundByConversationIdSocketEvent& messagesReadStateFoundByConversationIdSocketEvent) override;
};
