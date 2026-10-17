#include "deserializer/AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"
#include "views/ConversationView.h"

#include <vector>

// **********
// * PUBLIC *
// **********
AllOpenConversationsFoundByUserIdSocketEventPayload AllOpenConversationsFoundByUserIdSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);
    const std::vector<Json> conversationViewJsons = json.GetObjectArray("conversations");

    std::vector<ConversationView> conversationViews = {};
    for (const Json& conversationViewJson : conversationViewJsons)
    {
        ConversationView conversationView = ConvertJsonToConversationView(conversationViewJson);
        conversationViews.push_back(conversationView);
    }

    AllOpenConversationsFoundByUserIdSocketEventPayload allOpenConversationsFoundByUserIdSocketEventPayload = {};
    allOpenConversationsFoundByUserIdSocketEventPayload.conversations = conversationViews;

    return allOpenConversationsFoundByUserIdSocketEventPayload;
}
