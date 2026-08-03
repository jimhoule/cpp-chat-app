#include "deserializer/RegisterSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
RegisterSocketEventPayload RegisterSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    Json json = Json::Parse(jsonString);

    RegisterSocketEventPayload registerSocketEventPayload = {};
    registerSocketEventPayload.firstName = json.GetString("firstName");
    registerSocketEventPayload.lastName = json.GetString("lastName");
    registerSocketEventPayload.email = json.GetString("email");
    registerSocketEventPayload.password = json.GetString("password");
    
    return registerSocketEventPayload;
}
