#include "serializer/MessageCreatedSocketEventSerializer.h"

#include "json/Json.h"
#include "json/MessageJson.h"

// **********
// * PUBLIC *
// **********
std::string MessageCreatedSocketEventSerializer::Serialize(const MessageCreatedSocketEvent& messageCreatedSocketEvent)
{
    const Json messageViewJson = ConvertMessageViewToJson(messageCreatedSocketEvent.payload.message);

    Json payloadJson = {};
    payloadJson.Set("message", messageViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", messageCreatedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
