#include "deserializer/LogoutSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
LogoutSocketEventPayload LogoutSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    LogoutSocketEventPayload logoutSocketEventPayload = {};
    logoutSocketEventPayload.sessionId = json.GetString("sessionId");

    return logoutSocketEventPayload;
}
