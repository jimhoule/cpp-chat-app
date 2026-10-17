#include "serializer/RegisteredSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string RegisteredSocketEventSerializer::Serialize(const RegisteredSocketEvent& registeredSocketEvent)
{
    Json userViewJson = {};
    userViewJson.Set("id", registeredSocketEvent.payload.user.id);
    userViewJson.Set("email", registeredSocketEvent.payload.user.email);
    userViewJson.Set("firstName", registeredSocketEvent.payload.user.firstName);
    userViewJson.Set("lastName", registeredSocketEvent.payload.user.lastName);

    Json payloadJson = {};
    payloadJson.Set("sessionId", registeredSocketEvent.payload.sessionId);
    payloadJson.Set("user", userViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", registeredSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
