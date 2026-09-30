#include "serializer/UserOfflineSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string UserOfflineSocketEventSerializer::Serialize(const UserOfflineSocketEvent& userOfflineSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("userId", userOfflineSocketEvent.payload.userId);

    Json json = {};
    json.SetEnum<SocketEventName>("name", userOfflineSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
