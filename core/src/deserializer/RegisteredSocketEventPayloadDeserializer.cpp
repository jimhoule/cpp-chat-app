#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
RegisteredSocketEventPayload RegisteredSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);
    Json userViewJson = json.GetObject("user");

    RegisteredSocketEventPayload registeredSocketEventPayload = {};
    registeredSocketEventPayload.sessionId = json.GetString("sessionId");
    registeredSocketEventPayload.user.id = userViewJson.GetString("id");
    registeredSocketEventPayload.user.email = userViewJson.GetString("email");
    registeredSocketEventPayload.user.firstName = userViewJson.GetString("firstName");
    registeredSocketEventPayload.user.lastName = userViewJson.GetString("lastName");

    return registeredSocketEventPayload;
}
