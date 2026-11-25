#include "deserializer/FindMessagesReadStateByConversationIdSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
FindMessagesReadStateByConversationIdSocketEventPayload FindMessagesReadStateByConversationIdSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);

    FindMessagesReadStateByConversationIdSocketEventPayload findMessagesReadStateByConversationIdSocketEventPayload = {};
    findMessagesReadStateByConversationIdSocketEventPayload.conversationId = json.GetString("conversationId");

    return findMessagesReadStateByConversationIdSocketEventPayload;
}
