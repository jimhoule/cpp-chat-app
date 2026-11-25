#include "deserializer/MessagesReadStateFoundByConversationIdSocketEventPayloadDeserializer.h"

#include "json/Json.h"
#include "json/MessageJson.h"
#include "views/MessagesReadStateView.h"

#include <utility>
#include <vector>

// **********
// * PUBLIC *
// **********
MessagesReadStateFoundByConversationIdSocketEventPayload MessagesReadStateFoundByConversationIdSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json messagesReadStateViewJson = json.GetObject("messagesReadState");

    MessagesReadStateView messagesReadStateView = ConvertJsonToMessagesReadStateView(messagesReadStateViewJson);

    MessagesReadStateFoundByConversationIdSocketEventPayload messagesReadStateFoundByConversationIdSocketEventPayload = {};
    messagesReadStateFoundByConversationIdSocketEventPayload.messagesReadState = std::move(messagesReadStateView);

    return messagesReadStateFoundByConversationIdSocketEventPayload;
}
