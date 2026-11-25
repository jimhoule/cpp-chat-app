#pragma once

#include "models/Message.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class IMessagesRepository
{
public:
    struct MessagesCount
    {
        std::uint64_t count = 0;
        // NOTE: Newest message of the conversation from any sender (0 if the conversation has no messages)
        std::uint64_t latestMessagePosition = 0;
    };

    using AfterPositionsMap = std::unordered_map<std::string, std::uint64_t>;
    using MessagesCountsMap = std::unordered_map<std::string, MessagesCount>;

    // NOTE: Explicitly default the constructor (Optional but recommended for clarity)
    IMessagesRepository() = default;

    // NOTE: ALWAYS provide a public virtual destructor
    virtual ~IMessagesRepository() = default;

    // Disables copying and moving to prevent slicing
    IMessagesRepository(const IMessagesRepository&) = delete;
    IMessagesRepository& operator=(const IMessagesRepository&) = delete;

    // NOTE: Counts messages of conversation after afterPosition excluding excludedSenderId's own messages
    virtual MessagesCount CountByConversationIdAfter(const std::string& conversationId, const std::string& excludedSenderId, std::uint64_t afterPosition) = 0;
    /**
     * NOTES:
     *  - Same as CountByConversationIdAfter() but for many conversations in one pass (one entry per conversation in afterPositionsMap)
     *  - Expects each afterPosition to be 0 or the position of a message of that conversation
     */
    virtual MessagesCountsMap CountByConversationIdsAfter(const AfterPositionsMap& afterPositionsMap, const std::string& excludedSenderId) = 0;
    virtual Message Create(const Message& message) = 0;
    virtual std::vector<Message> FindByConversationIdBefore(const std::string& conversationId, const std::string& beforeMessageId, std::size_t limit) = 0;
    virtual std::optional<Message> FindById(const std::string& id) = 0;
};
