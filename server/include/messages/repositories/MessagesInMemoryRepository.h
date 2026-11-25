#pragma once

#include "messages/repositories/IMessagesRepository.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// Forward declarations
class Logger;

class MessagesInMemoryRepository : public IMessagesRepository
{
public:
    MessagesInMemoryRepository(Logger& logger);

    MessagesCount CountByConversationIdAfter(const std::string& conversationId, const std::string& excludedSenderId, std::uint64_t afterPosition) override;
    MessagesCountsMap CountByConversationIdsAfter(const AfterPositionsMap& afterPositionsMap, const std::string& excludedSenderId) override;
    Message Create(const Message& message) override;
    std::vector<Message> FindByConversationIdBefore(const std::string& conversationId, const std::string& beforeMessageId, std::size_t limit) override;
    std::optional<Message> FindById(const std::string& id) override;

private:
    std::vector<Message> m_messages = {};
    // NOTE: Next send order position (starts at 1 because 0 means nothing read)
    std::uint64_t m_nextPosition = 1;

    // NOTE: Borrowed from the module, which owns it and outlives this repository
    Logger& m_logger;
};
