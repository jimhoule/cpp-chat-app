#include "serializer/RegisteredSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string RegisteredSocketEventSerializer::Serialize(const RegisteredSocketEvent& registeredSocketEvent)
{
    Json userJson = {};
    userJson.Set("id", registeredSocketEvent.payload.user.id);
    userJson.Set("email", registeredSocketEvent.payload.user.email);
    userJson.Set("firstName", registeredSocketEvent.payload.user.firstName);
    userJson.Set("lastName", registeredSocketEvent.payload.user.lastName);
    userJson.Set("password", registeredSocketEvent.payload.user.password);

    Json payloadJson = {};
    payloadJson.Set("sessionId", registeredSocketEvent.payload.sessionId);
    payloadJson.Set("user", userJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", registeredSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
