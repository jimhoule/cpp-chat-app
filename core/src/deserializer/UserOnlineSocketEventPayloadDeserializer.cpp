#include "deserializer/UserOnlineSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
UserOnlineSocketEventPayload UserOnlineSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);
    Json userJson = json.GetObject("user");

    UserOnlineSocketEventPayload userOnlineSocketEventPayload = {};
    userOnlineSocketEventPayload.user.id = userJson.GetString("id");
    userOnlineSocketEventPayload.user.email = userJson.GetString("email");
    userOnlineSocketEventPayload.user.firstName = userJson.GetString("firstName");
    userOnlineSocketEventPayload.user.lastName = userJson.GetString("lastName");
    userOnlineSocketEventPayload.user.password = userJson.GetString("password");

    return userOnlineSocketEventPayload;
}
