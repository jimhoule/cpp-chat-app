#include "serializer/MessagesMarkedReadSocketEventSerializer.h"

#include "json/Json.h"
#include "json/MessageJson.h"

// **********
// * PUBLIC *
// **********
std::string MessagesMarkedReadSocketEventSerializer::Serialize(const MessagesMarkedReadSocketEvent& messagesMarkedReadSocketEvent)
{
    const Json messagesReadStateViewJson = ConvertMessagesReadStateViewToJson(messagesMarkedReadSocketEvent.payload.messagesReadState);

    Json payloadJson = {};
    payloadJson.Set("messagesReadState", messagesReadStateViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", messagesMarkedReadSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
