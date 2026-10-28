#include "deserializer/AuthenticatedSocketEventPayloadDeserializer.h"

#include "json/Json.h"
#include "json/UserJson.h"
#include "views/UserView.h"

// **********
// * PUBLIC *
// **********
AuthenticatedSocketEventPayload AuthenticatedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json userViewJson = json.GetObject("user");

    const UserView userView = ConvertJsonToUserView(userViewJson);

    AuthenticatedSocketEventPayload authenticatedSocketEventPayload = {};
    authenticatedSocketEventPayload.user = userView;

    return authenticatedSocketEventPayload;
}
