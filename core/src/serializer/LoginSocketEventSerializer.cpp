#include "serializer/LoginSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string LoginSocketEventSerializer::Serialize(const LoginSocketEvent& loginSocketEvent)
{
    nlohmann::json PayloadJson;
    PayloadJson["email"] = loginSocketEvent.Payload.email;
    PayloadJson["password"] = loginSocketEvent.Payload.password;

    nlohmann::json Json;
    Json["name"] = loginSocketEvent.Name;
    Json["payload"] = PayloadJson;

    return Json.dump();
}
