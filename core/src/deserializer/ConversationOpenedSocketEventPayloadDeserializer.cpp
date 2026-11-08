#include "deserializer/ConversationOpenedSocketEventPayloadDeserializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"
#include "views/ConversationView.h"

#include <utility>
#include <vector>

// **********
// * PUBLIC *
// **********
ConversationOpenedSocketEventPayload ConversationOpenedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json conversationViewJson = json.GetObject("conversation");

    ConversationView conversationView = ConvertJsonToConversationView(conversationViewJson);

    ConversationOpenedSocketEventPayload conversationOpenedSocketEventPayload = {};
    conversationOpenedSocketEventPayload.conversation = std::move(conversationView);

    return conversationOpenedSocketEventPayload;
}
