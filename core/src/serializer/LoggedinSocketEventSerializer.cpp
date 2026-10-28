#include "serializer/LoggedinSocketEventSerializer.h"

#include "json/Json.h"
#include "json/UserJson.h"

// **********
// * PUBLIC *
// **********
std::string LoggedinSocketEventSerializer::Serialize(const LoggedinSocketEvent& loggedinSocketEvent)
{
    const Json userViewJson = ConvertUserViewToJson(loggedinSocketEvent.payload.user);

    Json payloadJson = {};
    payloadJson.Set("sessionId", loggedinSocketEvent.payload.sessionId);
    payloadJson.Set("user", userViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", loggedinSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
