#include "serializer/RegisteredSocketEventSerializer.h"

#include "json/Json.h"
#include "json/UserJson.h"

// **********
// * PUBLIC *
// **********
std::string RegisteredSocketEventSerializer::Serialize(const RegisteredSocketEvent& registeredSocketEvent)
{
    Json userViewJson = ConvertUserViewToJson(registeredSocketEvent.payload.user);

    Json payloadJson = {};
    payloadJson.Set("sessionId", registeredSocketEvent.payload.sessionId);
    payloadJson.Set("user", userViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", registeredSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
