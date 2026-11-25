#include "json/MessageJson.h"

Json ConvertMessageViewToJson(const MessageView& messageView)
{
    Json messageViewJson = {};
    messageViewJson.Set("id", messageView.id);
    messageViewJson.Set("conversationId", messageView.conversationId);
    messageViewJson.Set("position", messageView.position);
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
    messageView.position = json.GetUint64("position");
    messageView.senderId = json.GetString("senderId");
    messageView.text = json.GetString("text");
    messageView.createdAt = json.GetTime("createdAt");

    return messageView;
}

Json ConvertMessagesReadStateViewToJson(const MessagesReadStateView& messagesReadStateView)
{
    Json messagesReadStateViewJson = {};
    messagesReadStateViewJson.Set("conversationId", messagesReadStateView.conversationId);
    messagesReadStateViewJson.Set("lastReadMessagePosition", messagesReadStateView.lastReadMessagePosition);
    messagesReadStateViewJson.Set("latestMessagePosition", messagesReadStateView.latestMessagePosition);
    messagesReadStateViewJson.Set("unreadCount", messagesReadStateView.unreadCount);

    return messagesReadStateViewJson;
}

MessagesReadStateView ConvertJsonToMessagesReadStateView(const Json& json)
{
    MessagesReadStateView messagesReadStateView = {};
    messagesReadStateView.conversationId = json.GetString("conversationId");
    messagesReadStateView.lastReadMessagePosition = json.GetUint64("lastReadMessagePosition");
    messagesReadStateView.latestMessagePosition = json.GetUint64("latestMessagePosition");
    messagesReadStateView.unreadCount = json.GetUint64("unreadCount");

    return messagesReadStateView;
}
