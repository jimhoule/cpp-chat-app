#include "deserializer/UserAuthenticatedSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
UserAuthenticatedSocketEventPayload UserAuthenticatedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);
    Json userJson = json.GetObject("user");

    UserAuthenticatedSocketEventPayload userAuthenticatedSocketEventPayload = {};
    userAuthenticatedSocketEventPayload.user.id = userJson.GetString("id");
    userAuthenticatedSocketEventPayload.user.email = userJson.GetString("email");
    userAuthenticatedSocketEventPayload.user.firstName = userJson.GetString("firstName");
    userAuthenticatedSocketEventPayload.user.lastName = userJson.GetString("lastName");
    userAuthenticatedSocketEventPayload.user.password = userJson.GetString("password");

    return userAuthenticatedSocketEventPayload;
}
