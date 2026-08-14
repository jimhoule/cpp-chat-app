#pragma once

#include "models/Conversation.h"

#include <optional>
#include <string>
#include <vector>

class IConversationsRepository
{
public:
    // NOTE: Explicitly default the constructor (Optional but recommended for clarity)
    IConversationsRepository() = default;

    // NOTE: ALWAYS provide a public virtual destructor
    virtual ~IConversationsRepository() = default;

    // Disables copying and moving to prevent slicing
    IConversationsRepository(const IConversationsRepository&) = delete;
    IConversationsRepository& operator=(const IConversationsRepository&) = delete;

    virtual Conversation Create(const Conversation& conversation) = 0;
    virtual std::optional<Conversation> FindById(const std::string& id) = 0;
    virtual std::optional<Conversation> FindByUserIds(const std::vector<std::string>& userIds) = 0;
    virtual std::vector<Conversation> FindAllOpenByUserId(const std::string& userId) = 0;
    virtual bool IsConversationUser(const std::string& conversationId, const std::string& userId) = 0;
    virtual std::optional<Conversation> SetIsOpen(const std::string& conversationId, const std::string& userId, bool isOpen) = 0;
};
