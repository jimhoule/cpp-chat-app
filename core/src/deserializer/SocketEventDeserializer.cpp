#include "deserializer/SocketEventDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
SocketEvent<std::string> SocketEventDeserializer::Deserialize(const std::string& JsonString)
{
    nlohmann::json Json = nlohmann::json::parse(JsonString);

    SocketEventName Name = Json["name"].get<SocketEventName>();
    std::string Payload = Json["payload"].dump();
    SocketEvent<std::string> SocketEvent(Name, Payload);

    return SocketEvent;
}
