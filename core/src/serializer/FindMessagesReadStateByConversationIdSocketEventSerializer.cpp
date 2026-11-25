#include "serializer/FindMessagesReadStateByConversationIdSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string FindMessagesReadStateByConversationIdSocketEventSerializer::Serialize(const FindMessagesReadStateByConversationIdSocketEvent& findMessagesReadStateByConversationIdSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("conversationId", findMessagesReadStateByConversationIdSocketEvent.payload.conversationId);

    Json json = {};
    json.SetEnum<SocketEventName>("name", findMessagesReadStateByConversationIdSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
