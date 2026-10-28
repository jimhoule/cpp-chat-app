#include "json/MessageJson.h"

Json ConvertMessageViewToJson(const MessageView& messageView)
{
    Json messageViewJson = {};
    messageViewJson.Set("id", messageView.id);
    messageViewJson.Set("conversationId", messageView.conversationId);
    messageViewJson.Set("senderId", messageView.senderId);
    messageViewJson.Set("text", messageView.text);
    messageViewJson.Set("createdAt", messageView.createdAt);

    return messageViewJson;
}

MessageView ConvertJsonToMessageView(const Json& json)
{
    MessageView messageView = {};
    messageView.id = json.GetString("id");
    messageView.conversationId = json.GetString("conversationId");
    messageView.senderId = json.GetString("senderId");
    messageView.text = json.GetString("text");
    messageView.createdAt = json.GetTime("createdAt");

    return messageView;
}
