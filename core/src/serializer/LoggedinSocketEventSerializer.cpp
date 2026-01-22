#include "serializer/LoggedinSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string LoggedinSocketEventSerializer::Serialize(const LoggedinSocketEvent& LoggedinSocketEvent)
{
    nlohmann::json PayloadJson;
    PayloadJson["accessToken"] = LoggedinSocketEvent.Payload.AccessToken;

    nlohmann::json Json;
    Json["name"] = LoggedinSocketEvent.Name;
    Json["payload"] = PayloadJson;

    return Json.dump();
}
