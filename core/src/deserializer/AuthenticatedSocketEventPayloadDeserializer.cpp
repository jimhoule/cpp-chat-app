#include "deserializer/AuthenticatedSocketEventPayloadDeserializer.h"

#include "json/Json.h"
#include "json/UserJson.h"
#include "views/UserView.h"

#include <utility>

// **********
// * PUBLIC *
// **********
AuthenticatedSocketEventPayload AuthenticatedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json userViewJson = json.GetObject("user");

    UserView userView = ConvertJsonToUserView(userViewJson);

    AuthenticatedSocketEventPayload authenticatedSocketEventPayload = {};
    authenticatedSocketEventPayload.user = std::move(userView);

    return authenticatedSocketEventPayload;
}
