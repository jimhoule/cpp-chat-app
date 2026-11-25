#pragma once

#include "json/Json.h"
#include "views/MessageView.h"
#include "views/MessagesReadStateView.h"

Json ConvertMessageViewToJson(const MessageView& messageView);
MessageView ConvertJsonToMessageView(const Json& json);

Json ConvertMessagesReadStateViewToJson(const MessagesReadStateView& messagesReadStateView);
MessagesReadStateView ConvertJsonToMessagesReadStateView(const Json& json);
