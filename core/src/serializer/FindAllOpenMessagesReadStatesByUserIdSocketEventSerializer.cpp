#include "serializer/FindAllOpenMessagesReadStatesByUserIdSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string FindAllOpenMessagesReadStatesByUserIdSocketEventSerializer::Serialize(const FindAllOpenMessagesReadStatesByUserIdSocketEvent& findAllOpenMessagesReadStatesByUserIdSocketEvent)
{
    // Sends empty payload
    const Json payloadJson = {};

    Json json = {};
    json.SetEnum<SocketEventName>("name", findAllOpenMessagesReadStatesByUserIdSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
