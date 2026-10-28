#include "serializer/AuthenticatedSocketEventSerializer.h"

#include "json/Json.h"
#include "json/UserJson.h"

// **********
// * PUBLIC *
// **********
std::string AuthenticatedSocketEventSerializer::Serialize(const AuthenticatedSocketEvent& authenticatedSocketEvent)
{
    const Json userViewJson = ConvertUserViewToJson(authenticatedSocketEvent.payload.user);

    Json payloadJson = {};
    payloadJson.Set("user", userViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", authenticatedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
