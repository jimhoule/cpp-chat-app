#include "json/ConversationJson.h"

#include "views/UserView.h"

#include <nlohmann/json.hpp>
#include <vector>

Json ConvertConversationViewToJson(const ConversationView& conversationView)
{
    std::vector<Json> conversationUserViewJsons = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    conversationUserViewJsons.reserve(conversationView.users.size());
    for (const ConversationUserView& conversationUserView : conversationView.users)
    {
        Json conversationUserViewJson = ConvertConversationUserViewToJson(conversationUserView);
        conversationUserViewJsons.push_back(conversationUserViewJson);
    }

    Json conversationViewJson = {};
    conversationViewJson.Set("id", conversationView.id);
    conversationViewJson.Set("users", conversationUserViewJsons);
    conversationViewJson.Set("createdAt", conversationView.createdAt);

    return conversationViewJson;
}

ConversationView ConvertJsonToConversationView(const Json& json)
{
    ConversationView conversationView = {};
    conversationView.id = json.GetString("id");
    conversationView.createdAt = json.GetTime("createdAt");

    const std::vector<Json> conversationUserViewJsons = json.GetObjectArray("users");
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    conversationView.users.reserve(conversationUserViewJsons.size());
    for (const Json& conversationUserViewJson : conversationUserViewJsons)
    {
        ConversationUserView conversationUserView = ConvertJsonToConversationUserView(conversationUserViewJson);
        conversationView.users.push_back(conversationUserView);
    }

    return conversationView;
}

Json ConvertConversationUserViewToJson(const ConversationUserView& conversationUserView)
{
    Json userViewJson = {};
    userViewJson.Set("id", conversationUserView.user.id);
    userViewJson.Set("email", conversationUserView.user.email);
    userViewJson.Set("firstName", conversationUserView.user.firstName);
    userViewJson.Set("lastName", conversationUserView.user.lastName);

    Json conversationUserViewJson = {};
    conversationUserViewJson.Set("user", userViewJson);
    conversationUserViewJson.Set("isOpen", conversationUserView.isOpen);

    return conversationUserViewJson;
}

ConversationUserView ConvertJsonToConversationUserView(const Json& json)
{
    Json userViewJson = json.GetObject("user");

    UserView userView = {};
    userView.id = userViewJson.GetString("id");
    userView.email = userViewJson.GetString("email");
    userView.firstName = userViewJson.GetString("firstName");
    userView.lastName = userViewJson.GetString("lastName");

    ConversationUserView conversationUserView = {};
    conversationUserView.user = userView;
    conversationUserView.isOpen = json.GetBool("isOpen");

    return conversationUserView;
}
