#include "deserializer/AuthenticateSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
AuthenticateSocketEventPayload AuthenticateSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);

    AuthenticateSocketEventPayload authenticateSocketEventPayload = {};
    authenticateSocketEventPayload.sessionId = json.GetString("sessionId");

    return authenticateSocketEventPayload;
}
