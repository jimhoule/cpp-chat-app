#include "messages/repositories/MessagesInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
MessagesInMemoryRepository::MessagesInMemoryRepository(Logger& logger)
    : m_logger(logger)
{}

Message MessagesInMemoryRepository::Create(const Message& message)
{
    m_messages.push_back(message);

    return message;
}