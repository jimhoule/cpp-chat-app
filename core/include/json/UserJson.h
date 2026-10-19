#pragma once

#include "json/Json.h"
#include "views/UserView.h"

Json ConvertUserViewToJson(const UserView& userView);
UserView ConvertJsonToUserView(const Json& json);
