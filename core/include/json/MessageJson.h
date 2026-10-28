#pragma once

#include "json/Json.h"
#include "views/MessageView.h"

Json ConvertMessageViewToJson(const MessageView& messageView);
MessageView ConvertJsonToMessageView(const Json& json);
