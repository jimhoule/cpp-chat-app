#include "deserializer/CreateMessageSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
CreateMessageSocketEventPayload CreateMessageSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    nlohmann::json json = nlohmann::json::parse(jsonString);

    CreateMessageSocketEventPayload createMessageSocketEventPayload = {};
    createMessageSocketEventPayload.conversationId = json["conversationId"].get<std::string>();
    createMessageSocketEventPayload.text = json["text"].get<std::string>();

    return createMessageSocketEventPayload;
}
