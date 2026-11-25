#include "deserializer/MessagesMarkedReadSocketEventPayloadDeserializer.h"

#include "json/Json.h"
#include "json/MessageJson.h"
#include "views/MessagesReadStateView.h"

#include <utility>
#include <vector>

// **********
// * PUBLIC *
// **********
MessagesMarkedReadSocketEventPayload MessagesMarkedReadSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json messagesReadStateViewJson = json.GetObject("messagesReadState");

    MessagesReadStateView messagesReadStateView = ConvertJsonToMessagesReadStateView(messagesReadStateViewJson);

    MessagesMarkedReadSocketEventPayload messagesMarkedReadSocketEventPayload = {};
    messagesMarkedReadSocketEventPayload.messagesReadState = std::move(messagesReadStateView);

    return messagesMarkedReadSocketEventPayload;
}
