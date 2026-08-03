#include "serializer/ErrorSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string ErrorSocketEventSerializer::Serialize(const ErrorSocketEvent& errorSocketEvent)
{
    Json payloadJson = {};
    payloadJson.SetEnum<SocketErrorCode>("code", errorSocketEvent.payload.code);

    Json json = {};
    json.SetEnum<SocketEventName>("name", errorSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
