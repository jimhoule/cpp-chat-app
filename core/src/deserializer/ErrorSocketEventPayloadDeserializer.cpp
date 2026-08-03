#include "deserializer/ErrorSocketEventPayloadDeserializer.h"
#include "socket/SocketErrorCode.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
ErrorSocketEventPayload ErrorSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    ErrorSocketEventPayload errorSocketEventPayload = {};
    errorSocketEventPayload.code = json.GetEnum<SocketErrorCode>("code");

    return errorSocketEventPayload;
}
