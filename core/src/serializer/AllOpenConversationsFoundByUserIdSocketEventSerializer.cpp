#include "serializer/AllOpenConversationsFoundByUserIdSocketEventSerializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"
#include "views/ConversationView.h"

#include <vector>

// **********
// * PUBLIC *
// **********
std::string AllOpenConversationsFoundByUserIdSocketEventSerializer::Serialize(const AllOpenConversationsFoundByUserIdSocketEvent& allOpenConversationsFoundByUserIdSocketEvent)
{
    // NOTE: Borrowed from the event, which outlives this call
    const std::vector<ConversationView>& conversationViews = allOpenConversationsFoundByUserIdSocketEvent.payload.conversations;

    std::vector<Json> conversationViewJsons = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    conversationViewJsons.reserve(conversationViews.size());
    for (const ConversationView& conversationView : conversationViews)
    {
        Json conversationViewJson = ConvertConversationViewToJson(conversationView);
        conversationViewJsons.push_back(conversationViewJson);
    }

    Json payloadJson = {};
    payloadJson.Set("conversations", conversationViewJsons);

    Json json = {};
    json.SetEnum<SocketEventName>("name", allOpenConversationsFoundByUserIdSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
