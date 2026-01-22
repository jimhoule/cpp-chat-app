#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
RegisteredSocketEventPayload RegisteredSocketEventPayloadDeserializer::Deserialize(const std::string& JsonString)
{
    nlohmann::json Json = nlohmann::json::parse(JsonString);

    RegisteredSocketEventPayload RegisteredSocketEventPayload = {};
    RegisteredSocketEventPayload.AccessToken = Json["accessToken"].get<std::string>();

    return RegisteredSocketEventPayload;
}
