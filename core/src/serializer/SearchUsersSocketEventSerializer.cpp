#include "serializer/SearchUsersSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string SearchUsersSocketEventSerializer::Serialize(const SearchUsersSocketEvent& searchUsersSocketEvent)
{
    Json payloadJson = {};
    payloadJson.Set("search", searchUsersSocketEvent.payload.search);

    Json json = {};
    json.SetEnum<SocketEventName>("name", searchUsersSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
