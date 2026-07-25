#pragma once

#include "messages/repositories/IMessagesRepository.h"

#include <vector>

// Forward declarations
class Logger;

class MessagesInMemoryRepository : public IMessagesRepository
{
public:
    MessagesInMemoryRepository(Logger& logger);

    Message Create(const Message& message) override;

private:
    std::vector<Message> m_messages = {};

    // NOTE: Borrowed from the module, which owns it and outlives this repository
    Logger& m_logger;
};
