#include "deserializer/SearchUsersSocketEventPayloadDeserializer.h"

#include "json/Json.h"

// **********
// * PUBLIC *
// **********
SearchUsersSocketEventPayload SearchUsersSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);

    SearchUsersSocketEventPayload searchUsersSocketEventPayload = {};
    searchUsersSocketEventPayload.search = json.GetString("search");

    return searchUsersSocketEventPayload;
}
