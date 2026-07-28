#include "serializer/ErrorSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string ErrorSocketEventSerializer::Serialize(const ErrorSocketEvent& errorSocketEvent)
{
    nlohmann::json payloadJson = {};
    payloadJson["code"] = errorSocketEvent.payload.code;

    nlohmann::json json = {};
    json["name"] = errorSocketEvent.name;
    json["payload"] = payloadJson;

    return json.dump();
}
