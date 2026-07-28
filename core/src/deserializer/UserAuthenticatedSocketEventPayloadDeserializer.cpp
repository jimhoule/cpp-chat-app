#include "deserializer/UserAuthenticatedSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
UserAuthenticatedSocketEventPayload UserAuthenticatedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    nlohmann::json json = nlohmann::json::parse(jsonString);
    nlohmann::json userJson = json["user"];

    UserAuthenticatedSocketEventPayload userAuthenticatedSocketEventPayload = {};
    userAuthenticatedSocketEventPayload.user.id = userJson["id"].get<std::string>();
    userAuthenticatedSocketEventPayload.user.email = userJson["email"].get<std::string>();
    userAuthenticatedSocketEventPayload.user.firstName = userJson["firstName"].get<std::string>();
    userAuthenticatedSocketEventPayload.user.lastName = userJson["lastName"].get<std::string>();
    userAuthenticatedSocketEventPayload.user.password = userJson["password"].get<std::string>();

    return userAuthenticatedSocketEventPayload;
}
