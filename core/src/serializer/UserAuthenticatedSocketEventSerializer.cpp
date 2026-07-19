#include "serializer/UserAuthenticatedSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string UserAuthenticatedSocketEventSerializer::Serialize(const UserAuthenticatedSocketEvent& userAuthenticatedSocketEvent)
{
    nlohmann::json userJson = {};
    userJson["id"] = userAuthenticatedSocketEvent.Payload.User.id;
    userJson["email"] = userAuthenticatedSocketEvent.Payload.User.email;
    userJson["firstName"] = userAuthenticatedSocketEvent.Payload.User.firstName;
    userJson["lastName"] = userAuthenticatedSocketEvent.Payload.User.lastName;
    userJson["password"] = userAuthenticatedSocketEvent.Payload.User.password;

    nlohmann::json payloadJson = {};
    payloadJson["user"] = userJson;

    nlohmann::json json = {};
    json["name"] = userAuthenticatedSocketEvent.Name;
    json["payload"] = payloadJson;

    return json.dump();
}
