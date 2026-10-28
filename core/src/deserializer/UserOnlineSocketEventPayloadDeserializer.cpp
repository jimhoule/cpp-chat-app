#include "deserializer/UserOnlineSocketEventPayloadDeserializer.h"

#include "json/Json.h"
#include "json/UserJson.h"
#include "views/UserView.h"

// **********
// * PUBLIC *
// **********
UserOnlineSocketEventPayload UserOnlineSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json userViewJson = json.GetObject("user");

    const UserView userView = ConvertJsonToUserView(userViewJson);

    UserOnlineSocketEventPayload userOnlineSocketEventPayload = {};
    userOnlineSocketEventPayload.user = userView;

    return userOnlineSocketEventPayload;
}
