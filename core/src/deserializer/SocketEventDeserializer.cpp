#include "deserializer/SocketEventDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
SocketEvent<std::string> SocketEventDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);

    const SocketEventName name = json.GetEnum<SocketEventName>("name");
    const std::string payload = json.GetRaw("payload");
    SocketEvent<std::string> socketEvent(name, payload);

    return socketEvent;
}
