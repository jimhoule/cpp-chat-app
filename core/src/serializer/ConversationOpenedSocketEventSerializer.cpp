#include "serializer/ConversationOpenedSocketEventSerializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string ConversationOpenedSocketEventSerializer::Serialize(const ConversationOpenedSocketEvent& conversationOpenedSocketEvent)
{
    Json conversationJson = ConvertConversationToJson(conversationOpenedSocketEvent.payload.conversation);

    Json payloadJson = {};
    payloadJson.Set("conversation", conversationJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", conversationOpenedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
