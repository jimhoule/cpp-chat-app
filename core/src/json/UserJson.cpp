#include "json/UserJson.h"

Json ConvertUserViewToJson(const UserView& userView)
{
    Json userViewJson = {};
    userViewJson.Set("id", userView.id);
    userViewJson.Set("email", userView.email);
    userViewJson.Set("firstName", userView.firstName);
    userViewJson.Set("lastName", userView.lastName);

    return userViewJson;
}

UserView ConvertJsonToUserView(const Json& json)
{
    UserView userView = {};
    userView.id = json.GetString("id");
    userView.email = json.GetString("email");
    userView.firstName = json.GetString("firstName");
    userView.lastName = json.GetString("lastName");

    return userView;
}
