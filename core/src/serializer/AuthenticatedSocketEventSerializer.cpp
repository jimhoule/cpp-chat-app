#include "serializer/AuthenticatedSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string AuthenticatedSocketEventSerializer::Serialize(const AuthenticatedSocketEvent& authenticatedSocketEvent)
{
    Json userJson = {};
    userJson.Set("id", authenticatedSocketEvent.payload.user.id);
    userJson.Set("email", authenticatedSocketEvent.payload.user.email);
    userJson.Set("firstName", authenticatedSocketEvent.payload.user.firstName);
    userJson.Set("lastName", authenticatedSocketEvent.payload.user.lastName);
    userJson.Set("password", authenticatedSocketEvent.payload.user.password);

    Json payloadJson = {};
    payloadJson.Set("user", userJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", authenticatedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
