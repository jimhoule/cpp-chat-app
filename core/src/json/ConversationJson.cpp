#include "json/ConversationJson.h"

#include <nlohmann/json.hpp>

Json ConvertConversationToJson(const Conversation& conversation)
{
    std::vector<Json> conversationUserJsons = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    conversationUserJsons.reserve(conversation.users.size());
    for (const ConversationUser& conversationUser : conversation.users)
    {
        Json conversationUserJson = ConvertConversationUserToJson(conversationUser);
        conversationUserJsons.push_back(conversationUserJson);
    }

    Json conversationJson = {};
    conversationJson.Set("id", conversation.id);
    conversationJson.Set("users", conversationUserJsons);
    conversationJson.Set("createdAt", conversation.createdAt);

    return conversationJson;
}

Conversation ConvertJsonToConversation(const Json& json)
{
    Conversation conversation = {};
    conversation.id = json.GetString("id");
    conversation.createdAt = json.GetTime("createdAt");

    const std::vector<Json> conversationUserJsons = json.GetObjectArray("users");
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    conversation.users.reserve(conversationUserJsons.size());
    for (const Json& conversationUserJson : conversationUserJsons)
    {
        conversation.users.push_back(ConvertJsonToConversationUser(conversationUserJson));
    }

    return conversation;
}

Json ConvertConversationUserToJson(const ConversationUser& conversationUser)
{
    Json conversationUserJson = {};
    conversationUserJson.Set("userId", conversationUser.userId);
    conversationUserJson.Set("isOpen", conversationUser.isOpen);

    return conversationUserJson;
}

ConversationUser ConvertJsonToConversationUser(const Json& json)
{
    ConversationUser conversationUser = {};
    conversationUser.userId = json.GetString("userId");
    conversationUser.isOpen = json.GetBool("isOpen");

    return conversationUser;
}
