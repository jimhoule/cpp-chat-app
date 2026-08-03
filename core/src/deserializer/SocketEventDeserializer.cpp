#include "deserializer/SocketEventDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
SocketEvent<std::string> SocketEventDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    SocketEventName name = json.GetEnum<SocketEventName>("name");
    std::string payload = json.GetRaw("payload");
    SocketEvent<std::string> socketEvent(name, payload);

    return socketEvent;
}
