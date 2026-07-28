#include "serializer/LoginSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string LoginSocketEventSerializer::Serialize(const LoginSocketEvent& loginSocketEvent)
{
    nlohmann::json payloadJson;
    payloadJson["email"] = loginSocketEvent.payload.email;
    payloadJson["password"] = loginSocketEvent.payload.password;

    nlohmann::json json;
    json["name"] = loginSocketEvent.name;
    json["payload"] = payloadJson;

    return json.dump();
}
