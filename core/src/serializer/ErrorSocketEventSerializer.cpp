#include "serializer/ErrorSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string ErrorSocketEventSerializer::Serialize(const ErrorSocketEvent& errorSocketEvent)
{
    Json payloadJson = {};
    payloadJson.SetEnum<SocketErrorCode>("code", errorSocketEvent.payload.code);
    payloadJson.SetEnum<SocketEventName>("failedEventName", errorSocketEvent.payload.failedEventName);

    Json json = {};
    json.SetEnum<SocketEventName>("name", errorSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
