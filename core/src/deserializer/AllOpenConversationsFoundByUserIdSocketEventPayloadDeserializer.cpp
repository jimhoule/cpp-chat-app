#include "deserializer/AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"
#include "views/ConversationView.h"

#include <utility>
#include <vector>

// **********
// * PUBLIC *
// **********
AllOpenConversationsFoundByUserIdSocketEventPayload AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const std::vector<Json> conversationViewJsons = json.GetObjectArray("conversations");

    std::vector<ConversationView> conversationViews = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    conversationViews.reserve(conversationViewJsons.size());
    for (const Json& conversationViewJson : conversationViewJsons)
    {
        ConversationView conversationView = ConvertJsonToConversationView(conversationViewJson);
        conversationViews.push_back(std::move(conversationView));
    }

    AllOpenConversationsFoundByUserIdSocketEventPayload allOpenConversationsFoundByUserIdSocketEventPayload = {};
    allOpenConversationsFoundByUserIdSocketEventPayload.conversations = std::move(conversationViews);

    return allOpenConversationsFoundByUserIdSocketEventPayload;
}
