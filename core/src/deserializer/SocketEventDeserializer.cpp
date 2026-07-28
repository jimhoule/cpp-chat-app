#include "deserializer/SocketEventDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
SocketEvent<std::string> SocketEventDeserializer::Deserialize(const std::string& jsonString)
{
    nlohmann::json json = nlohmann::json::parse(jsonString);

    SocketEventName name = json["name"].get<SocketEventName>();
    std::string payload = json["payload"].dump();
    SocketEvent<std::string> socketEvent(name, payload);

    return socketEvent;
}
