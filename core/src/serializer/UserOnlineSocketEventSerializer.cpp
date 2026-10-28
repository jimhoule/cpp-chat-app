#include "serializer/UserOnlineSocketEventSerializer.h"

#include "json/Json.h"
#include "json/UserJson.h"

// **********
// * PUBLIC *
// **********
std::string UserOnlineSocketEventSerializer::Serialize(const UserOnlineSocketEvent& userOnlineSocketEvent)
{
    const Json userViewJson = ConvertUserViewToJson(userOnlineSocketEvent.payload.user);

    Json payloadJson = {};
    payloadJson.Set("user", userViewJson);

    Json json = {};
    json.SetEnum<SocketEventName>("name", userOnlineSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
