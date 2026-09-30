#include "serializer/LogoutSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string LogoutSocketEventSerializer::Serialize(const LogoutSocketEvent& logoutSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("sessionId", logoutSocketEvent.payload.sessionId);

    Json json = {};
    json.SetEnum<SocketEventName>("name", logoutSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
