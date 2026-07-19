#include "serializer/RegisterSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string RegisterSocketEventSerializer::Serialize(const RegisterSocketEvent& registerSocketEvent)
{
    nlohmann::json payloadJson;
    payloadJson["firstName"] = registerSocketEvent.Payload.firstName;
    payloadJson["lastName"] = registerSocketEvent.Payload.lastName;
    payloadJson["email"] = registerSocketEvent.Payload.email;
    payloadJson["password"] = registerSocketEvent.Payload.password;

    nlohmann::json json;
    json["name"] = registerSocketEvent.Name;
    json["payload"] = payloadJson;

    return json.dump();
}
