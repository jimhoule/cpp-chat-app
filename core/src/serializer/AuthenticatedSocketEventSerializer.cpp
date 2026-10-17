#include "serializer/AuthenticatedSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string AuthenticatedSocketEventSerializer::Serialize(const AuthenticatedSocketEvent& authenticatedSocketEvent)
{
    Json userViewJson = {};
    userViewJson.Set("id", authenticatedSocketEvent.payload.user.id);
    userViewJson.Set("email", authenticatedSocketEvent.payload.user.email);
    userViewJson.Set("firstName", authenticatedSocketEvent.payload.user.firstName);
    userViewJson.Set("lastName", authenticatedSocketEvent.payload.user.lastName);

    Json payloadJson = {};
    payloadJson.Set("user", userViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", authenticatedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
