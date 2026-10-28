#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"

#include "json/Json.h"
#include "json/UserJson.h"
#include "views/UserView.h"

// **********
// * PUBLIC *
// **********
LoggedinSocketEventPayload LoggedinSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json userViewJson = json.GetObject("user");

    const UserView userView = ConvertJsonToUserView(userViewJson);

    LoggedinSocketEventPayload loggedinSocketEventPayload = {};
    loggedinSocketEventPayload.sessionId = json.GetString("sessionId");
    loggedinSocketEventPayload.user = userView;

    return loggedinSocketEventPayload;
}
