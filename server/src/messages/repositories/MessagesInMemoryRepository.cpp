#include "messages/repositories/MessagesInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
Message MessagesInMemoryRepository::Create(const Message& message)
{
    m_messages.push_back(message);

    return message;
}