#include "serializer/ConversationFoundByUserIdsSocketEventSerializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string ConversationFoundByUserIdsSocketEventSerializer::Serialize(const ConversationFoundByUserIdsSocketEvent& conversationFoundByUserIdsSocketEvent)
{
    Json payloadJson = {};
    if (conversationFoundByUserIdsSocketEvent.payload.conversation.has_value())
    {
        Json conversationJson = ConvertConversationToJson(conversationFoundByUserIdsSocketEvent.payload.conversation.value());
        payloadJson.Set("conversation", conversationJson);
    }
    else
    {
        // NOTE: No conversation for these users yet is a normal answer, not an error
        payloadJson.SetNull("conversation");
    }

    Json json = {};
    json.SetEnum<SocketEventName>("name", conversationFoundByUserIdsSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
