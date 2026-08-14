#include "serializer/ConversationClosedSocketEventSerializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string ConversationClosedSocketEventSerializer::Serialize(const ConversationClosedSocketEvent& conversationClosedSocketEvent)
{
    Json conversationJson = ConvertConversationToJson(conversationClosedSocketEvent.payload.conversation);

    Json payloadJson = {};
    payloadJson.Set("conversation", conversationJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", conversationClosedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
