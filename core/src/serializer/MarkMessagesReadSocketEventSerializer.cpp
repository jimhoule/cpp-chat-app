#include "serializer/MarkMessagesReadSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string MarkMessagesReadSocketEventSerializer::Serialize(const MarkMessagesReadSocketEvent& markMessagesReadSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("conversationId", markMessagesReadSocketEvent.payload.conversationId);
    payloadJson.Set("messageId", markMessagesReadSocketEvent.payload.messageId);

    Json json = {};
    json.SetEnum<SocketEventName>("name", markMessagesReadSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
