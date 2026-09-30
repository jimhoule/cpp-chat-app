#include "serializer/LoggedoutSocketEventSerializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
std::string LoggedoutSocketEventSerializer::Serialize(const LoggedoutSocketEvent& loggedoutSocketEvent)
{
    // Sends empty payload
    Json payloadJson = {};

    Json json = {};
    json.SetEnum<SocketEventName>("name", loggedoutSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
