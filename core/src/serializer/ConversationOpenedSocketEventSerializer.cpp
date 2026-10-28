#include "serializer/ConversationOpenedSocketEventSerializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string ConversationOpenedSocketEventSerializer::Serialize(const ConversationOpenedSocketEvent& conversationOpenedSocketEvent)
{
    const Json conversationViewJson = ConvertConversationViewToJson(conversationOpenedSocketEvent.payload.conversation);

    Json payloadJson = {};
    payloadJson.Set("conversation", conversationViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", conversationOpenedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
