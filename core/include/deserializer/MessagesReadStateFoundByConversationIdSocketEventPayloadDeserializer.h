#pragma once

#include "deserializer/IDeserializer.h"
#include "socket/MessagesReadStateFoundByConversationIdSocketEvent.h"

class MessagesReadStateFoundByConversationIdSocketEventPayloadDeserializer: public IDeserializer<std::string, MessagesReadStateFoundByConversationIdSocketEventPayload>
{
public:
    MessagesReadStateFoundByConversationIdSocketEventPayloadDeserializer() = default;

    MessagesReadStateFoundByConversationIdSocketEventPayload Deserialize(const std::string& jsonString) override;
};
