#include "serializer/ConversationReopenedSocketEventSerializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string ConversationReopenedSocketEventSerializer::Serialize(const ConversationReopenedSocketEvent& conversationReopenedSocketEvent)
{
    const Json conversationViewJson = ConvertConversationViewToJson(conversationReopenedSocketEvent.payload.conversation);

    Json payloadJson = {};
    payloadJson.Set("conversation", conversationViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", conversationReopenedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
