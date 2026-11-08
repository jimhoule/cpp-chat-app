#include "deserializer/MessagesPageFoundByConversationIdSocketEventPayloadDeserializer.h"

#include "json/MessageJson.h"
#include "json/PageJson.h"
#include "json/Json.h"
#include "views/MessageView.h"
#include "views/MessagesPageView.h"

#include <utility>
#include <vector>

// **********
// * PUBLIC *
// **********
MessagesPageFoundByConversationIdSocketEventPayload MessagesPageFoundByConversationIdSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json messagesPageViewJson = json.GetObject("messagesPage");

    MessagesPageView messagesPageView = ConvertJsonToPageView<MessageView, decltype(&ConvertJsonToMessageView)>(messagesPageViewJson, ConvertJsonToMessageView);
    
    MessagesPageFoundByConversationIdSocketEventPayload messagesPageFoundByConversationIdSocketEventPayload = {};
    messagesPageFoundByConversationIdSocketEventPayload.messagesPage = std::move(messagesPageView);
    messagesPageFoundByConversationIdSocketEventPayload.conversationId = json.GetString("conversationId");

    return messagesPageFoundByConversationIdSocketEventPayload;
}
