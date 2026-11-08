#include "deserializer/FindMessagesPageByConversationIdSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
FindMessagesPageByConversationIdSocketEventPayload FindMessagesPageByConversationIdSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);

    FindMessagesPageByConversationIdSocketEventPayload findMessagesPageByConversationIdSocketEventPayload = {};
    findMessagesPageByConversationIdSocketEventPayload.conversationId = json.GetString("conversationId");
    findMessagesPageByConversationIdSocketEventPayload.cursor = json.GetString("cursor");

    return findMessagesPageByConversationIdSocketEventPayload;
}
