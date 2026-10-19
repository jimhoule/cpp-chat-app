#include "deserializer/ConversationClosedSocketEventPayloadDeserializer.h"

#include "json/ConversationJson.h"
#include "json/Json.h"
#include "views/ConversationView.h"

#include <vector>

// **********
// * PUBLIC *
// **********
ConversationClosedSocketEventPayload ConversationClosedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json conversationViewJson = json.GetObject("conversation");

    ConversationView conversationView = ConvertJsonToConversationView(conversationViewJson);

    ConversationClosedSocketEventPayload conversationClosedSocketEventPayload = {};
    conversationClosedSocketEventPayload.conversation = conversationView;

    return conversationClosedSocketEventPayload;
}
