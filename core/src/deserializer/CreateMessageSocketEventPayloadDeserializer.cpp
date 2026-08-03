#include "deserializer/CreateMessageSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
CreateMessageSocketEventPayload CreateMessageSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    CreateMessageSocketEventPayload createMessageSocketEventPayload = {};
    createMessageSocketEventPayload.conversationId = json.GetString("conversationId");
    createMessageSocketEventPayload.text = json.GetString("text");

    return createMessageSocketEventPayload;
}
