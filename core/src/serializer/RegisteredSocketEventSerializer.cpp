#include "serializer/RegisteredSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string RegisteredSocketEventSerializer::Serialize(const RegisteredSocketEvent& RegisteredSocketEvent)
{
    nlohmann::json PayloadJson;
    PayloadJson["accessToken"] = RegisteredSocketEvent.Payload.AccessToken;

    nlohmann::json Json;
    Json["name"] = RegisteredSocketEvent.Name;
    Json["payload"] = PayloadJson;

    return Json.dump();
}
