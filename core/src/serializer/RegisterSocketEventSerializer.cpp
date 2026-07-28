#include "serializer/RegisterSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string RegisterSocketEventSerializer::Serialize(const RegisterSocketEvent& registerSocketEvent)
{
    nlohmann::json payloadJson;
    payloadJson["firstName"] = registerSocketEvent.payload.firstName;
    payloadJson["lastName"] = registerSocketEvent.payload.lastName;
    payloadJson["email"] = registerSocketEvent.payload.email;
    payloadJson["password"] = registerSocketEvent.payload.password;

    nlohmann::json json;
    json["name"] = registerSocketEvent.name;
    json["payload"] = payloadJson;

    return json.dump();
}
