#include "deserializer/OpenConversationSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
OpenConversationSocketEventPayload OpenConversationSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    OpenConversationSocketEventPayload openConversationSocketEventPayload = {};
    openConversationSocketEventPayload.userIds = json.GetStringArray("userIds");

    return openConversationSocketEventPayload;
}
