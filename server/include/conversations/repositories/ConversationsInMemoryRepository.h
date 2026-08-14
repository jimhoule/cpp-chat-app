#pragma once

#include "conversations/repositories/IConversationsRepository.h"
#include "models/Conversation.h"

#include <optional>
#include <string>
#include <vector>

// Forward declarations
class Logger;

class ConversationsInMemoryRepository : public IConversationsRepository
{
public:
    ConversationsInMemoryRepository(Logger& logger);

    Conversation Create(const Conversation& conversation) override;
    std::optional<Conversation> FindById(const std::string& id) override;
    std::optional<Conversation> FindByUserIds(const std::vector<std::string>& userIds) override;
    std::vector<Conversation> FindAllOpenByUserId(const std::string& userId) override;
    bool IsConversationUser(const std::string& conversationId, const std::string& userId) override;
    std::optional<Conversation> SetIsOpen(const std::string& conversationId, const std::string& userId, bool isOpen) override;

private:
    std::vector<Conversation> m_conversations = {};

    // NOTE: Borrowed from the module, which owns it and outlives this repository
    Logger& m_logger;

    /**
     * NOTES:
     *  - Returns pointers into m_conversations so callers can mutate what is stored, nullptr on a miss
     *  - Only valid until the next Create, which can reallocate m_conversations (right now non of the functions using those ones call Create() so it is fine but it is a rule to remember)
     */
    Conversation* FindConversationPointer(const std::string& conversationId);
    ConversationUser* FindConversationUserPointer(Conversation& conversation, const std::string& userId);
};
