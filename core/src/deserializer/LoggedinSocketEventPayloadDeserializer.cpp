#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
LoggedinSocketEventPayload LoggedinSocketEventPayloadDeserializer::Deserialize(const std::string& JsonString)
{
    nlohmann::json Json = nlohmann::json::parse(JsonString);

    LoggedinSocketEventPayload LoggedinSocketEventPayload = {};
    LoggedinSocketEventPayload.AccessToken = Json["accessToken"].get<std::string>();

    return LoggedinSocketEventPayload;
}
