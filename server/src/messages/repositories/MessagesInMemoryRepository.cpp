#include "messages/repositories/MessagesInMemoryRepository.h"

#include "log/Logger.h"

#include <algorithm>

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

std::vector<Message> MessagesInMemoryRepository::FindByConversationIdBefore(const std::string& conversationId, const std::string& beforeMessageId, std::size_t limit)
{
    /**
     * NOTES:
     *  - m_messages is append only (insertion order is send order)
     *  - Walks the vector backward using crbegin() (newest to last)
     *  - ++messagesReverseIterator moves to an older message
     */
    std::vector<Message>::const_reverse_iterator messagesReverseIterator = m_messages.crbegin();

    // Starts from the newest message if there is no cursor (first page)
    if (!beforeMessageId.empty())
    {
        // Finds the cursor message (it must belong to this conversation)
        messagesReverseIterator = std::find_if(
            m_messages.crbegin(),
            m_messages.crend(),
            [&beforeMessageId, &conversationId](const Message& message) {
                bool isCursorMessage = message.id == beforeMessageId && message.conversationId == conversationId;

                return isCursorMessage;
            }
        );

        if (messagesReverseIterator == m_messages.crend())
        {
            m_logger.Error("Message " + beforeMessageId + " not found in conversation " + conversationId);
            return {};
        }

        // Skips the cursor message itself because the client already has it
        ++messagesReverseIterator;
    }

    // Continues from where it points until no messages are left or the page is full
    std::vector<Message> conversationMessages = {};
    while (messagesReverseIterator != m_messages.crend() && conversationMessages.size() < limit)
    {
        const Message& message = *messagesReverseIterator;

        if (message.conversationId == conversationId)
        {
            conversationMessages.push_back(message);
        }

        ++messagesReverseIterator;
    }

    // Reverses messages order from last to newest
    std::reverse(conversationMessages.begin(), conversationMessages.end());

    return conversationMessages;
}
