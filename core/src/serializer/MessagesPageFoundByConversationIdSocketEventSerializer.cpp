#include "serializer/MessagesPageFoundByConversationIdSocketEventSerializer.h"

#include "json/MessageJson.h"
#include "json/PageJson.h"
#include "json/Json.h"
#include "views/MessageView.h"
#include "views/MessagesPageView.h"

#include <vector>

// **********
// * PUBLIC *
// **********
std::string MessagesPageFoundByConversationIdSocketEventSerializer::Serialize(const MessagesPageFoundByConversationIdSocketEvent& messagesPageFoundByConversationIdSocketEvent)
{
    // NOTE: Borrowed from the event, which outlives this call
    const MessagesPageView& messagesPageView = messagesPageFoundByConversationIdSocketEvent.payload.messagesPage;

    const Json messagesPageViewJson = ConvertPageViewToJson<MessageView, decltype(&ConvertMessageViewToJson)>(messagesPageView, ConvertMessageViewToJson);

    Json payloadJson = {};
    payloadJson.Set("messagesPage", messagesPageViewJson);
    payloadJson.Set("conversationId", messagesPageFoundByConversationIdSocketEvent.payload.conversationId);

    Json json = {};
    json.SetEnum<SocketEventName>("name", messagesPageFoundByConversationIdSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
