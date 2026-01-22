#include "deserializer/LoginSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
LoginSocketEventPayload LoginSocketEventPayloadDeserializer::Deserialize(const std::string& JsonString)
{
    nlohmann::json Json = nlohmann::json::parse(JsonString);

    LoginSocketEventPayload LoginSocketEventPayload = {};
    LoginSocketEventPayload.Email = Json["email"].get<std::string>();
    LoginSocketEventPayload.Password = Json["password"].get<std::string>();

    return LoginSocketEventPayload;
}
