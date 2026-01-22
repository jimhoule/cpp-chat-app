#include "deserializer/RegisterSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
RegisterSocketEventPayload RegisterSocketEventPayloadDeserializer::Deserialize(const std::string& JsonString)
{
    nlohmann::json Json = nlohmann::json::parse(JsonString);

    RegisterSocketEventPayload RegisterSocketEventPayload = {};
    RegisterSocketEventPayload.FirstName = Json["firstName"].get<std::string>();
    RegisterSocketEventPayload.LastName = Json["lastName"].get<std::string>();
    RegisterSocketEventPayload.Email = Json["email"].get<std::string>();
    RegisterSocketEventPayload.Password = Json["password"].get<std::string>();

    return RegisterSocketEventPayload;
}
