#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
LoggedinSocketEventPayload LoggedinSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);
    Json userViewJson = json.GetObject("user");

    LoggedinSocketEventPayload loggedinSocketEventPayload = {};
    loggedinSocketEventPayload.sessionId = json.GetString("sessionId");
    loggedinSocketEventPayload.user.id = userViewJson.GetString("id");
    loggedinSocketEventPayload.user.email = userViewJson.GetString("email");
    loggedinSocketEventPayload.user.firstName = userViewJson.GetString("firstName");
    loggedinSocketEventPayload.user.lastName = userViewJson.GetString("lastName");

    return loggedinSocketEventPayload;
}
