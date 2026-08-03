#include "serializer/CreateMessageSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string CreateMessageSocketEventSerializer::Serialize(const CreateMessageSocketEvent& createMessageSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("conversationId", createMessageSocketEvent.payload.conversationId);
    payloadJson.Set("text", createMessageSocketEvent.payload.text);

    Json json = {};
    json.SetEnum<SocketEventName>("name", createMessageSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
