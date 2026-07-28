#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
LoggedinSocketEventPayload LoggedinSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    nlohmann::json json = nlohmann::json::parse(jsonString);

    LoggedinSocketEventPayload loggedinSocketEventPayload = {};
    loggedinSocketEventPayload.sessionId = json["sessionId"].get<std::string>();

    return loggedinSocketEventPayload;
}
