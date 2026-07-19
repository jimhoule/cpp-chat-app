#pragma once

#include "messages/repositories/IMessagesRepository.h"

#include <vector>

class MessagesInMemoryRepository : public IMessagesRepository
{
public:
    MessagesInMemoryRepository() = default;

    Message Create(const Message& message) override;

private:
    std::vector<Message> m_messages = {};
};
