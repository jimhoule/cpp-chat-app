#pragma once

#include "models/Message.h"

#include <cstddef>
#include <string>
#include <vector>

class IMessagesRepository
{
public:
    // NOTE: Explicitly default the constructor (Optional but recommended for clarity)
    IMessagesRepository() = default;

    // NOTE: ALWAYS provide a public virtual destructor
    virtual ~IMessagesRepository() = default;

    // Disables copying and moving to prevent slicing
    IMessagesRepository(const IMessagesRepository&) = delete;
    IMessagesRepository& operator=(const IMessagesRepository&) = delete;

    virtual Message Create(const Message& message) = 0;
    virtual std::vector<Message> FindByConversationIdBefore(const std::string& conversationId, const std::string& beforeMessageId, std::size_t limit) = 0;
};
