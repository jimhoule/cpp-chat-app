#include "serializer/RegisteredSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string RegisteredSocketEventSerializer::Serialize(const RegisteredSocketEvent& registeredSocketEvent)
{
    nlohmann::json payloadJson;
    payloadJson["sessionId"] = registeredSocketEvent.payload.sessionId;

    nlohmann::json json;
    json["name"] = registeredSocketEvent.name;
    json["payload"] = payloadJson;

    return json.dump();
}
