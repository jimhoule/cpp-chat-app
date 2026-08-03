#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
LoggedinSocketEventPayload LoggedinSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    LoggedinSocketEventPayload loggedinSocketEventPayload = {};
    loggedinSocketEventPayload.sessionId = json.GetString("sessionId");

    return loggedinSocketEventPayload;
}
