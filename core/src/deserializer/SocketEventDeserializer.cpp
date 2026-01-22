#include "deserializer/SocketEventDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
SocketEvent<std::string> SocketEventDeserializer::Deserialize(const std::string& JsonString)
{
    nlohmann::json Json = nlohmann::json::parse(JsonString);

    SocketEvent<std::string> SocketEvent = {};
    SocketEvent.Name = Json["name"].get<SocketEventName>();
    SocketEvent.Payload = Json["payload"].dump();

    return SocketEvent;
}
