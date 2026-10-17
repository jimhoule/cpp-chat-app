#include "serializer/UserOnlineSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string UserOnlineSocketEventSerializer::Serialize(const UserOnlineSocketEvent& userOnlineSocketEvent)
{
    Json userViewJson = {};
    userViewJson.Set("id", userOnlineSocketEvent.payload.user.id);
    userViewJson.Set("email", userOnlineSocketEvent.payload.user.email);
    userViewJson.Set("firstName", userOnlineSocketEvent.payload.user.firstName);
    userViewJson.Set("lastName", userOnlineSocketEvent.payload.user.lastName);

    Json payloadJson = {};
    payloadJson.Set("user", userViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", userOnlineSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
