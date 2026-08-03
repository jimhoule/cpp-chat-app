#include "serializer/UserAuthenticatedSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string UserAuthenticatedSocketEventSerializer::Serialize(const UserAuthenticatedSocketEvent& userAuthenticatedSocketEvent)
{
    Json userJson = {};
    userJson.Set("id", userAuthenticatedSocketEvent.payload.user.id);
    userJson.Set("email", userAuthenticatedSocketEvent.payload.user.email);
    userJson.Set("firstName", userAuthenticatedSocketEvent.payload.user.firstName);
    userJson.Set("lastName", userAuthenticatedSocketEvent.payload.user.lastName);
    userJson.Set("password", userAuthenticatedSocketEvent.payload.user.password);

    Json payloadJson = {};
    payloadJson.Set("user", userJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", userAuthenticatedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
