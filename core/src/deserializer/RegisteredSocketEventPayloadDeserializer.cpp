#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
RegisteredSocketEventPayload RegisteredSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    RegisteredSocketEventPayload registeredSocketEventPayload = {};
    registeredSocketEventPayload.sessionId = json.GetString("sessionId");

    return registeredSocketEventPayload;
}
