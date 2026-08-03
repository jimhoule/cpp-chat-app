#include "serializer/LoginSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string LoginSocketEventSerializer::Serialize(const LoginSocketEvent& loginSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("email", loginSocketEvent.payload.email);
    payloadJson.Set("password", loginSocketEvent.payload.password);

    Json json = {};
    json.SetEnum<SocketEventName>("name", loginSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
