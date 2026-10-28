#include "deserializer/UserOfflineSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
UserOfflineSocketEventPayload UserOfflineSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);

    UserOfflineSocketEventPayload userOfflineSocketEventPayload = {};
    userOfflineSocketEventPayload.userId = json.GetString("userId");

    return userOfflineSocketEventPayload;
}
