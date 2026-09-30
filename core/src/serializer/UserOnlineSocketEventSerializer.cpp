#include "serializer/UserOnlineSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string UserOnlineSocketEventSerializer::Serialize(const UserOnlineSocketEvent& userOnlineSocketEvent)
{
    Json userJson = {};
    userJson.Set("id", userOnlineSocketEvent.payload.user.id);
    userJson.Set("email", userOnlineSocketEvent.payload.user.email);
    userJson.Set("firstName", userOnlineSocketEvent.payload.user.firstName);
    userJson.Set("lastName", userOnlineSocketEvent.payload.user.lastName);
    userJson.Set("password", userOnlineSocketEvent.payload.user.password);

    Json payloadJson = {};
    payloadJson.Set("user", userJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", userOnlineSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
