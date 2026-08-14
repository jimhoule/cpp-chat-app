#pragma once

#include "json/Json.h"
#include "models/Conversation.h"
#include "models/ConversationUser.h"

Json ConvertConversationToJson(const Conversation& conversation);
Conversation ConvertJsonToConversation(const Json& json);

Json ConvertConversationUserToJson(const ConversationUser& conversationUser);
ConversationUser ConvertJsonToConversationUser(const Json& json);