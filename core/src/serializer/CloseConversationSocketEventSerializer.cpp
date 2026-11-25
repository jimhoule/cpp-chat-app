#include "serializer/CloseConversationSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string CloseConversationSocketEventSerializer::Serialize(const CloseConversationSocketEvent& closeConversationSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("id", closeConversationSocketEvent.payload.id);

    Json json = {};
    json.SetEnum<SocketEventName>("name", closeConversationSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
