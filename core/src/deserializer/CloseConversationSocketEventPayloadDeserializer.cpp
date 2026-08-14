#include "deserializer/CloseConversationSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
CloseConversationSocketEventPayload CloseConversationSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    CloseConversationSocketEventPayload closeConversationSocketEventPayload = {};
    closeConversationSocketEventPayload.conversationId = json.GetString("conversationId");

    return closeConversationSocketEventPayload;
}
