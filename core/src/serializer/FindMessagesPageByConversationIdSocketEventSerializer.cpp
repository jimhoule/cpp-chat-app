#include "serializer/FindMessagesPageByConversationIdSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string FindMessagesPageByConversationIdSocketEventSerializer::Serialize(const FindMessagesPageByConversationIdSocketEvent& findMessagesPageByConversationIdSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("conversationId", findMessagesPageByConversationIdSocketEvent.payload.conversationId);
    payloadJson.Set("cursor", findMessagesPageByConversationIdSocketEvent.payload.cursor);

    Json json = {};
    json.SetEnum<SocketEventName>("name", findMessagesPageByConversationIdSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
