#include "serializer/OpenConversationSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string OpenConversationSocketEventSerializer::Serialize(const OpenConversationSocketEvent& openConversationSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("userIds", openConversationSocketEvent.payload.userIds);

    Json json = {};
    json.SetEnum<SocketEventName>("name", openConversationSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
