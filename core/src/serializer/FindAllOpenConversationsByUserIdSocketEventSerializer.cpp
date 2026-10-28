#include "serializer/FindAllOpenConversationsByUserIdSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string FindAllOpenConversationsByUserIdSocketEventSerializer::Serialize(const FindAllOpenConversationsByUserIdSocketEvent& findAllOpenConversationsByUserIdSocketEvent)
{
    // Sends empty payload
    const Json payloadJson = {};

    Json json = {};
    json.SetEnum<SocketEventName>("name", findAllOpenConversationsByUserIdSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
