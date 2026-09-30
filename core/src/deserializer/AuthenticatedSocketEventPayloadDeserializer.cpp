#include "deserializer/AuthenticatedSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
AuthenticatedSocketEventPayload AuthenticatedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);
    Json userJson = json.GetObject("user");

    AuthenticatedSocketEventPayload authenticatedSocketEventPayload = {};
    authenticatedSocketEventPayload.user.id = userJson.GetString("id");
    authenticatedSocketEventPayload.user.email = userJson.GetString("email");
    authenticatedSocketEventPayload.user.firstName = userJson.GetString("firstName");
    authenticatedSocketEventPayload.user.lastName = userJson.GetString("lastName");
    authenticatedSocketEventPayload.user.password = userJson.GetString("password");

    return authenticatedSocketEventPayload;
}
