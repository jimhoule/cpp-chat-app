#include "deserializer/ConversationReopenedSocketEventPayloadDeserializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"
#include "views/ConversationView.h"

#include <utility>
#include <vector>

// **********
// * PUBLIC *
// **********
ConversationReopenedSocketEventPayload ConversationReopenedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json conversationViewJson = json.GetObject("conversation");

    ConversationView conversationView = ConvertJsonToConversationView(conversationViewJson);

    ConversationReopenedSocketEventPayload conversationReopenedSocketEventPayload = {};
    conversationReopenedSocketEventPayload.conversation = std::move(conversationView);

    return conversationReopenedSocketEventPayload;
}
