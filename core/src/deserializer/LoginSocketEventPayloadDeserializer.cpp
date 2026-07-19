#include "deserializer/LoginSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
LoginSocketEventPayload LoginSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    nlohmann::json json = nlohmann::json::parse(jsonString);

    LoginSocketEventPayload loginSocketEventPayload = {};
    loginSocketEventPayload.email = json["email"].get<std::string>();
    loginSocketEventPayload.password = json["password"].get<std::string>();

    return loginSocketEventPayload;
}
