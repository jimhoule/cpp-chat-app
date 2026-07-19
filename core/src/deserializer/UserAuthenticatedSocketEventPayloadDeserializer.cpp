#include "deserializer/UserAuthenticatedSocketEventPayloadDeserializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
UserAuthenticatedSocketEventPayload UserAuthenticatedSocketEventPayloadDeserializer::Deserialize(const std::string& JsonString)
{
    nlohmann::json Json = nlohmann::json::parse(JsonString);
    nlohmann::json UserJson = Json["user"];

    UserAuthenticatedSocketEventPayload UserAuthenticatedSocketEventPayload = {};
    UserAuthenticatedSocketEventPayload.User.id = UserJson["id"].get<std::string>();
    UserAuthenticatedSocketEventPayload.User.email = UserJson["email"].get<std::string>();
    UserAuthenticatedSocketEventPayload.User.firstName = UserJson["firstName"].get<std::string>();
    UserAuthenticatedSocketEventPayload.User.lastName = UserJson["lastName"].get<std::string>();
    UserAuthenticatedSocketEventPayload.User.password = UserJson["password"].get<std::string>();

    return UserAuthenticatedSocketEventPayload;
}
