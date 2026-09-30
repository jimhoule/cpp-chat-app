#include "serializer/AuthenticateSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string AuthenticateSocketEventSerializer::Serialize(const AuthenticateSocketEvent& authenticateSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("sessionId", authenticateSocketEvent.payload.sessionId);

    Json json = {};
    json.SetEnum<SocketEventName>("name", authenticateSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
