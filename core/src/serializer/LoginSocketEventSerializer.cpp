#include "serializer/LoginSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string LoginSocketEventSerializer::Serialize(const LoginSocketEvent& LoginSocketEvent)
{
    nlohmann::json PayloadJson;
    PayloadJson["email"] = LoginSocketEvent.Payload.Email;
    PayloadJson["password"] = LoginSocketEvent.Payload.Password;

    nlohmann::json Json;
    Json["name"] = LoginSocketEvent.Name;
    Json["payload"] = PayloadJson;

    return Json.dump();
}
