#pragma once

#include "deserializer/IDeserializer.h"
#include "socket/MessagesPageFoundByConversationIdSocketEvent.h"

class MessagesPageFoundByConversationIdSocketEventPayloadDeserializer: public IDeserializer<std::string, MessagesPageFoundByConversationIdSocketEventPayload>
{
public:
    MessagesPageFoundByConversationIdSocketEventPayloadDeserializer() = default;

    MessagesPageFoundByConversationIdSocketEventPayload Deserialize(const std::string& jsonString) override;
};
