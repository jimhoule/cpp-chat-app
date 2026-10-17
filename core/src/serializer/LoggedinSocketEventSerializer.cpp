#include "serializer/LoggedinSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string LoggedinSocketEventSerializer::Serialize(const LoggedinSocketEvent& loggedinSocketEvent)
{
    Json userViewJson = {};
    userViewJson.Set("id", loggedinSocketEvent.payload.user.id);
    userViewJson.Set("email", loggedinSocketEvent.payload.user.email);
    userViewJson.Set("firstName", loggedinSocketEvent.payload.user.firstName);
    userViewJson.Set("lastName", loggedinSocketEvent.payload.user.lastName);

    Json payloadJson = {};
    payloadJson.Set("sessionId", loggedinSocketEvent.payload.sessionId);
    payloadJson.Set("user", userViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", loggedinSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
