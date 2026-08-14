#include "serializer/AllOpenConversationsFoundByUserIdSocketEventSerializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string AllOpenConversationsFoundByUserIdSocketEventSerializer::Serialize(const AllOpenConversationsFoundByUserIdSocketEvent& allOpenConversationsFoundByUserIdSocketEvent)
{
    // NOTE: Borrowed from the event, which outlives this call
    const std::vector<Conversation>& conversations = allOpenConversationsFoundByUserIdSocketEvent.payload.conversations;

    std::vector<Json> conversationJsons = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    conversationJsons.reserve(conversations.size());
    for (const Conversation& conversation : conversations)
    {
        Json conversationJson = ConvertConversationToJson(conversation);
        conversationJsons.push_back(conversationJson);
    }

    Json payloadJson = {};
    payloadJson.Set("conversations", conversationJsons);

    Json json = {};
    json.SetEnum<SocketEventName>("name", allOpenConversationsFoundByUserIdSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
