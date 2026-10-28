#include "deserializer/UsersSearchedSocketEventPayloadDeserializer.h"

#include "json/UserJson.h"
#include "json/Json.h"
#include "views/UserView.h"

#include <vector>

// **********
// * PUBLIC *
// **********
UsersSearchedSocketEventPayload UsersSearchedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const std::vector<Json> userViewJsons = json.GetObjectArray("users");

    std::vector<UserView> userViews = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    userViews.reserve(userViewJsons.size());
    for (const Json& userViewJson : userViewJsons)
    {
        const UserView userView = ConvertJsonToUserView(userViewJson);
        userViews.push_back(userView);
    }

    UsersSearchedSocketEventPayload usersSearchedSocketEventPayload = {};
    usersSearchedSocketEventPayload.users = userViews;

    return usersSearchedSocketEventPayload;
}
