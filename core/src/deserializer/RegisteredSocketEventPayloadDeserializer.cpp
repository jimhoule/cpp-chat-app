#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
RegisteredSocketEventPayload RegisteredSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);
    Json userJson = json.GetObject("user");

    RegisteredSocketEventPayload registeredSocketEventPayload = {};
    registeredSocketEventPayload.sessionId = json.GetString("sessionId");
    registeredSocketEventPayload.user.id = userJson.GetString("id");
    registeredSocketEventPayload.user.email = userJson.GetString("email");
    registeredSocketEventPayload.user.firstName = userJson.GetString("firstName");
    registeredSocketEventPayload.user.lastName = userJson.GetString("lastName");
    registeredSocketEventPayload.user.password = userJson.GetString("password");

    return registeredSocketEventPayload;
}
