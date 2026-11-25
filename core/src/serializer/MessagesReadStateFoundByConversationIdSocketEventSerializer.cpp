#include "serializer/MessagesReadStateFoundByConversationIdSocketEventSerializer.h"

#include "json/Json.h"
#include "json/MessageJson.h"

// **********
// * PUBLIC *
// **********
std::string MessagesReadStateFoundByConversationIdSocketEventSerializer::Serialize(const MessagesReadStateFoundByConversationIdSocketEvent& messagesReadStateFoundByConversationIdSocketEvent)
{
    const Json messagesReadStateViewJson = ConvertMessagesReadStateViewToJson(messagesReadStateFoundByConversationIdSocketEvent.payload.messagesReadState);

    Json payloadJson = {};
    payloadJson.Set("messagesReadState", messagesReadStateViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", messagesReadStateFoundByConversationIdSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
