#include "deserializer/LoginSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
LoginSocketEventPayload LoginSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    LoginSocketEventPayload loginSocketEventPayload = {};
    loginSocketEventPayload.email = json.GetString("email");
    loginSocketEventPayload.password = json.GetString("password");

    return loginSocketEventPayload;
}
