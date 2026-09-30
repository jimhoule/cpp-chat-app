#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
LoggedinSocketEventPayload LoggedinSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);
    Json userJson = json.GetObject("user");

    LoggedinSocketEventPayload loggedinSocketEventPayload = {};
    loggedinSocketEventPayload.sessionId = json.GetString("sessionId");
    loggedinSocketEventPayload.user.id = userJson.GetString("id");
    loggedinSocketEventPayload.user.email = userJson.GetString("email");
    loggedinSocketEventPayload.user.firstName = userJson.GetString("firstName");
    loggedinSocketEventPayload.user.lastName = userJson.GetString("lastName");
    loggedinSocketEventPayload.user.password = userJson.GetString("password");

    return loggedinSocketEventPayload;
}
