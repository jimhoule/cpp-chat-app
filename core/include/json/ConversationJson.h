#pragma once

#include "json/Json.h"
#include "views/ConversationView.h"
#include "views/ConversationUserView.h"

Json ConvertConversationViewToJson(const ConversationView& conversationView);
ConversationView ConvertJsonToConversationView(const Json& json);

Json ConvertConversationUserViewToJson(const ConversationUserView& conversationUserView);
ConversationUserView ConvertJsonToConversationUserView(const Json& json);