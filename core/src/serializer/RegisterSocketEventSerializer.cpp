#include "serializer/RegisterSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string RegisterSocketEventSerializer::Serialize(const RegisterSocketEvent& registerSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("firstName", registerSocketEvent.payload.firstName);
    payloadJson.Set("lastName", registerSocketEvent.payload.lastName);
    payloadJson.Set("email", registerSocketEvent.payload.email);
    payloadJson.Set("password", registerSocketEvent.payload.password);

    Json json = {};
    json.SetEnum<SocketEventName>("name", registerSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
