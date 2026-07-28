#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
RegisteredSocketEventPayload RegisteredSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    nlohmann::json json = nlohmann::json::parse(jsonString);

    RegisteredSocketEventPayload registeredSocketEventPayload = {};
    registeredSocketEventPayload.sessionId = json["sessionId"].get<std::string>();

    return registeredSocketEventPayload;
}
