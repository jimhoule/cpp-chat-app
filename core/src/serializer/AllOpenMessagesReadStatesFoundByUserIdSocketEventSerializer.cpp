#include "serializer/AllOpenMessagesReadStatesFoundByUserIdSocketEventSerializer.h"

#include "json/Json.h"
#include "json/MessageJson.h"
#include "views/MessagesReadStateView.h"

#include <vector>

// **********
// * PUBLIC *
// **********
std::string AllOpenMessagesReadStatesFoundByUserIdSocketEventSerializer::Serialize(const AllOpenMessagesReadStatesFoundByUserIdSocketEvent& allOpenMessagesReadStatesFoundByUserIdSocketEvent)
{
    // NOTE: Borrowed from the event, which outlives this call
    const std::vector<MessagesReadStateView>& messagesReadStateViews = allOpenMessagesReadStatesFoundByUserIdSocketEvent.payload.messagesReadStates;

    std::vector<Json> messagesReadStateViewJsons = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    messagesReadStateViewJsons.reserve(messagesReadStateViews.size());
    for (const MessagesReadStateView& messagesReadStateView : messagesReadStateViews)
    {
        const Json messagesReadStateViewJson = ConvertMessagesReadStateViewToJson(messagesReadStateView);
        messagesReadStateViewJsons.push_back(messagesReadStateViewJson);
    }

    Json payloadJson = {};
    payloadJson.Set("messagesReadStates", messagesReadStateViewJsons);

    Json json = {};
    json.SetEnum<SocketEventName>("name", allOpenMessagesReadStatesFoundByUserIdSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
