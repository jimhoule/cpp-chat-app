#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"

#include "json/Json.h"
#include "json/UserJson.h"
#include "views/UserView.h"

// **********
// * PUBLIC *
// **********
RegisteredSocketEventPayload RegisteredSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json userViewJson = json.GetObject("user");

    const UserView userView = ConvertJsonToUserView(userViewJson);

    RegisteredSocketEventPayload registeredSocketEventPayload = {};
    registeredSocketEventPayload.sessionId = json.GetString("sessionId");
    registeredSocketEventPayload.user = userView;

    return registeredSocketEventPayload;
}
