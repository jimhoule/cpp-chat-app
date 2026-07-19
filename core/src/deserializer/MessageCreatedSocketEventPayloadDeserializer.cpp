#include "deserializer/MessageCreatedSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
MessageCreatedSocketEventPayload MessageCreatedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    nlohmann::json json = nlohmann::json::parse(jsonString);
    nlohmann::json messageJson = json["message"];

    MessageCreatedSocketEventPayload messageCreatedSocketEventPayload = {};
    messageCreatedSocketEventPayload.message.id = messageJson["id"].get<std::string>();
    messageCreatedSocketEventPayload.message.conversationId = messageJson["conversationId"].get<std::string>();
    messageCreatedSocketEventPayload.message.senderId = messageJson["senderId"].get<std::string>();
    messageCreatedSocketEventPayload.message.text = messageJson["text"].get<std::string>();
    messageCreatedSocketEventPayload.message.createdAt = messageJson["createdAt"].get<std::time_t>();

    return messageCreatedSocketEventPayload;
}
