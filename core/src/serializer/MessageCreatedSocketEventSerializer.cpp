#include "serializer/MessageCreatedSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string MessageCreatedSocketEventSerializer::Serialize(const MessageCreatedSocketEvent& messageCreatedSocketEvent)
{
    Json messageJson = {};
    messageJson.Set("id", messageCreatedSocketEvent.payload.message.id);
    messageJson.Set("conversationId", messageCreatedSocketEvent.payload.message.conversationId);
    messageJson.Set("senderId", messageCreatedSocketEvent.payload.message.senderId);
    messageJson.Set("text", messageCreatedSocketEvent.payload.message.text);
    messageJson.Set("createdAt", messageCreatedSocketEvent.payload.message.createdAt);

    Json payloadJson = {};
    payloadJson.Set("message", messageJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", messageCreatedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
