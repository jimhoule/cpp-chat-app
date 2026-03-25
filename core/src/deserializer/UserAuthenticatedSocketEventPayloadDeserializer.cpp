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
    UserAuthenticatedSocketEventPayload.User.ID = UserJson["id"].get<std::string>();
    UserAuthenticatedSocketEventPayload.User.FirstName = UserJson["firstName"].get<std::string>();
    UserAuthenticatedSocketEventPayload.User.LastName = UserJson["lastName"].get<std::string>();
    UserAuthenticatedSocketEventPayload.User.ImageUrl = UserJson["imageUrl"].get<std::string>();

    return UserAuthenticatedSocketEventPayload;
}
