#include "serializer/RegisteredSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string RegisteredSocketEventSerializer::Serialize(const RegisteredSocketEvent& registeredSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("sessionId", registeredSocketEvent.payload.sessionId);

    Json json = {};
    json.SetEnum<SocketEventName>("name", registeredSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
