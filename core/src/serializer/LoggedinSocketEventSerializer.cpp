#include "serializer/LoggedinSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string LoggedinSocketEventSerializer::Serialize(const LoggedinSocketEvent& loggedinSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("sessionId", loggedinSocketEvent.payload.sessionId);

    Json json = {};
    json.SetEnum<SocketEventName>("name", loggedinSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
