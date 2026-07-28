#include "serializer/LoggedinSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string LoggedinSocketEventSerializer::Serialize(const LoggedinSocketEvent& loggedinSocketEvent)
{
    nlohmann::json payloadJson;
    payloadJson["sessionId"] = loggedinSocketEvent.payload.sessionId;

    nlohmann::json json;
    json["name"] = loggedinSocketEvent.name;
    json["payload"] = payloadJson;

    return json.dump();
}
