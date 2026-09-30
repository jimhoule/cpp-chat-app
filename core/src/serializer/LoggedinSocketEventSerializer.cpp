#include "serializer/LoggedinSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string LoggedinSocketEventSerializer::Serialize(const LoggedinSocketEvent& loggedinSocketEvent)
{
    Json userJson = {};
    userJson.Set("id", loggedinSocketEvent.payload.user.id);
    userJson.Set("email", loggedinSocketEvent.payload.user.email);
    userJson.Set("firstName", loggedinSocketEvent.payload.user.firstName);
    userJson.Set("lastName", loggedinSocketEvent.payload.user.lastName);
    userJson.Set("password", loggedinSocketEvent.payload.user.password);

    Json payloadJson = {};
    payloadJson.Set("sessionId", loggedinSocketEvent.payload.sessionId);
    payloadJson.Set("user", userJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", loggedinSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
