#include "deserializer/FindConversationByUserIdsSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
FindConversationByUserIdsSocketEventPayload FindConversationByUserIdsSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    FindConversationByUserIdsSocketEventPayload findConversationByUserIdsSocketEventPayload = {};
    findConversationByUserIdsSocketEventPayload.userIds = json.GetStringArray("userIds");

    return findConversationByUserIdsSocketEventPayload;
}
