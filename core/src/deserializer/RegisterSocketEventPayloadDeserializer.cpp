#include "deserializer/RegisterSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
RegisterSocketEventPayload RegisterSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    nlohmann::json json = nlohmann::json::parse(jsonString);

    RegisterSocketEventPayload registerSocketEventPayload = {};
    registerSocketEventPayload.firstName = json["firstName"].get<std::string>();
    registerSocketEventPayload.lastName = json["lastName"].get<std::string>();
    registerSocketEventPayload.email = json["email"].get<std::string>();
    registerSocketEventPayload.password = json["password"].get<std::string>();

    return registerSocketEventPayload;
}
