#include "serializer/RegisterSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string RegisterSocketEventSerializer::Serialize(const RegisterSocketEvent& RegisterSocketEvent)
{
    nlohmann::json PayloadJson;
    PayloadJson["firstName"] = RegisterSocketEvent.Payload.FirstName;
    PayloadJson["lastName"] = RegisterSocketEvent.Payload.LastName;
    PayloadJson["email"] = RegisterSocketEvent.Payload.Email;
    PayloadJson["password"] = RegisterSocketEvent.Payload.Password;

    nlohmann::json Json;
    Json["name"] = RegisterSocketEvent.Name;
    Json["payload"] = PayloadJson;

    return Json.dump();
}
