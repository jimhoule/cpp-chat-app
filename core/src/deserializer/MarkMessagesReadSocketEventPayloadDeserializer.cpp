#include "deserializer/MarkMessagesReadSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
MarkMessagesReadSocketEventPayload MarkMessagesReadSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);

    MarkMessagesReadSocketEventPayload markMessagesReadSocketEventPayload = {};
    markMessagesReadSocketEventPayload.conversationId = json.GetString("conversationId");
    markMessagesReadSocketEventPayload.messageId = json.GetString("messageId");

    return markMessagesReadSocketEventPayload;
}
