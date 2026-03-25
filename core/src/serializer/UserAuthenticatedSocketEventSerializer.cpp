#include "serializer/UserAuthenticatedSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string UserAuthenticatedSocketEventSerializer::Serialize(const UserAuthenticatedSocketEvent& UserAuthenticatedSocketEvent)
{
    nlohmann::json UserJson = {};
    UserJson["id"] = UserAuthenticatedSocketEvent.Payload.User.ID;
    UserJson["firstName"] = UserAuthenticatedSocketEvent.Payload.User.FirstName;
    UserJson["lastName"] = UserAuthenticatedSocketEvent.Payload.User.LastName;
    UserJson["imageUrl"] = UserAuthenticatedSocketEvent.Payload.User.ImageUrl;

    nlohmann::json PayloadJson = {};
    PayloadJson["user"] = UserJson;

    nlohmann::json Json = {};
    Json["name"] = UserAuthenticatedSocketEvent.Name;
    Json["payload"] = PayloadJson;

    return Json.dump();
}
