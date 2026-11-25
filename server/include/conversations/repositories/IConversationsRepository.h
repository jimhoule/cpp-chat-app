#pragma once

#include "models/Conversation.h"

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class IConversationsRepository
{
public:
    using LastReadMessagePositionsMap = std::unordered_map<std::string, std::uint64_t>;

    // NOTE: Explicitly default the constructor (Optional but recommended for clarity)
    IConversationsRepository() = default;

    // NOTE: ALWAYS provide a public virtual destructor
    virtual ~IConversationsRepository() = default;

    // Disables copying and moving to prevent slicing
    IConversationsRepository(const IConversationsRepository&) = delete;
    IConversationsRepository& operator=(const IConversationsRepository&) = delete;

    virtual std::optional<Conversation> AdvanceLastReadMessagePosition(
        const std::string& id,
        const std::string& userId,
        std::uint64_t messagePosition
    ) = 0;
    virtual Conversation Create(const Conversation& conversation) = 0;
    virtual std::vector<Conversation> FindAllOpenByUserId(const std::string& userId) = 0;
    virtual LastReadMessagePositionsMap FindAllOpenLastReadMessagePositionsByUserId(const std::string& userId) = 0;
    virtual std::optional<Conversation> FindById(const std::string& id) = 0;
    virtual std::optional<Conversation> FindByUserIds(const std::vector<std::string>& userIds) = 0;
    virtual std::optional<std::uint64_t> FindLastReadMessagePositionById(const std::string& id, const std::string& userId) = 0;
    virtual bool IsConversationUser(const std::string& id, const std::string& userId) = 0;
    virtual std::optional<Conversation> UpdateIsOpen(const std::string& id, const std::string& userId, bool isOpen) = 0;
};
