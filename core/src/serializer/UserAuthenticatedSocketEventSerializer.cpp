#include "serializer/UserAuthenticatedSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string UserAuthenticatedSocketEventSerializer::Serialize(const UserAuthenticatedSocketEvent& userAuthenticatedSocketEvent)
{
    nlohmann::json userJson = {};
    userJson["id"] = userAuthenticatedSocketEvent.payload.user.id;
    userJson["email"] = userAuthenticatedSocketEvent.payload.user.email;
    userJson["firstName"] = userAuthenticatedSocketEvent.payload.user.firstName;
    userJson["lastName"] = userAuthenticatedSocketEvent.payload.user.lastName;
    userJson["password"] = userAuthenticatedSocketEvent.payload.user.password;

    nlohmann::json payloadJson = {};
    payloadJson["user"] = userJson;

    nlohmann::json json = {};
    json["name"] = userAuthenticatedSocketEvent.name;
    json["payload"] = payloadJson;

    return json.dump();
}
