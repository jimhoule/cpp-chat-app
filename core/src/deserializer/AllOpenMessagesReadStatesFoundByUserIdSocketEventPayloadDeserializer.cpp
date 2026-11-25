#include "deserializer/AllOpenMessagesReadStatesFoundByUserIdSocketEventPayloadDeserializer.h"

#include "json/Json.h"
#include "json/MessageJson.h"
#include "views/MessagesReadStateView.h"

#include <utility>
#include <vector>

// **********
// * PUBLIC *
// **********
AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload AllOpenMessagesReadStatesFoundByUserIdSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const std::vector<Json> messagesReadStateViewJsons = json.GetObjectArray("messagesReadStates");

    std::vector<MessagesReadStateView> messagesReadStateViews = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    messagesReadStateViews.reserve(messagesReadStateViewJsons.size());
    for (const Json& messagesReadStateViewJson : messagesReadStateViewJsons)
    {
        MessagesReadStateView messagesReadStateView = ConvertJsonToMessagesReadStateView(messagesReadStateViewJson);
        messagesReadStateViews.push_back(std::move(messagesReadStateView));
    }

    AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload allOpenMessagesReadStatesFoundByUserIdSocketEventPayload = {};
    allOpenMessagesReadStatesFoundByUserIdSocketEventPayload.messagesReadStates = std::move(messagesReadStateViews);

    return allOpenMessagesReadStatesFoundByUserIdSocketEventPayload;
}
