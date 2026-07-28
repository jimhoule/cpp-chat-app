#include "deserializer/ErrorSocketEventPayloadDeserializer.h"
#include "socket/SocketErrorCode.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
ErrorSocketEventPayload ErrorSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    nlohmann::json json = nlohmann::json::parse(jsonString);

    ErrorSocketEventPayload errorSocketEventPayload = {};
    errorSocketEventPayload.code = json["code"].get<SocketErrorCode>();

    return errorSocketEventPayload;
}
