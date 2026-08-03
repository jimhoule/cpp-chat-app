#include "deserializer/MessageCreatedSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
MessageCreatedSocketEventPayload MessageCreatedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);
    Json messageJson = json.GetObject("message");

    MessageCreatedSocketEventPayload messageCreatedSocketEventPayload = {};
    messageCreatedSocketEventPayload.message.id = messageJson.GetString("id");
    messageCreatedSocketEventPayload.message.conversationId = messageJson.GetString("conversationId");
    messageCreatedSocketEventPayload.message.senderId = messageJson.GetString("senderId");
    messageCreatedSocketEventPayload.message.text = messageJson.GetString("text");
    messageCreatedSocketEventPayload.message.createdAt = messageJson.GetTime("createdAt");

    return messageCreatedSocketEventPayload;
}
