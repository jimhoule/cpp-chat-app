#include "serializer/UsersSearchedSocketEventSerializer.h"

#include "json/UserJson.h"
#include "json/Json.h"
#include "views/UserView.h"

#include <vector>

// **********
// * PUBLIC *
// **********
std::string UsersSearchedSocketEventSerializer::Serialize(const UsersSearchedSocketEvent& usersSearchedSocketEvent)
{
    // NOTE: Borrowed from the event, which outlives this call
    const std::vector<UserView>& userViews = usersSearchedSocketEvent.payload.users;

    std::vector<Json> userViewJsons = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    userViewJsons.reserve(userViews.size());
    for (const UserView& userView : userViews)
    {
        const Json userViewJson = ConvertUserViewToJson(userView);
        userViewJsons.push_back(userViewJson);
    }

    Json payloadJson = {};
    payloadJson.Set("users", userViewJsons);

    Json json = {};
    json.SetEnum<SocketEventName>("name", usersSearchedSocketEvent.name);
    json.Set("payload", payloadJson);

    return json.ToString();
}
