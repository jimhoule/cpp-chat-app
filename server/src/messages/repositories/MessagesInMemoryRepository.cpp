#include "messages/repositories/MessagesInMemoryRepository.h"

#include "log/Logger.h"

#include <algorithm>
#include <limits>

// **********
// * PUBLIC *
// **********
MessagesInMemoryRepository::MessagesInMemoryRepository(Logger& logger)
    : m_logger(logger)
{}

MessagesInMemoryRepository::MessagesCount MessagesInMemoryRepository::CountByConversationIdAfter(const std::string& conversationId, const std::string& excludedSenderId, std::uint64_t afterPosition)
{
    MessagesCount messagesCount = {};

    /**
     * NOTES:
     *  - m_messages is in position order so the loop stops at the first message of the conversation at or before
     *    afterPosition (already read and everything older is too)
     *  - First message of conversation met is the newest one (latestMessagePosition)
     */
    for (std::vector<Message>::const_reverse_iterator messagesReverseIterator = m_messages.crbegin(); messagesReverseIterator != m_messages.crend(); ++messagesReverseIterator)
    {
        const Message& message = *messagesReverseIterator;

        // Checks if message belongs to conversation
        if (message.conversationId != conversationId)
        {
            continue;
        }

        /**
         * NOTES:
         *  - Saves position of the first message met for this conversation (it is the newest one because the walk goes from newest to oldest)
         *  - Real messages never have position 0 because positions start at 1
         */
        if (messagesCount.latestMessagePosition == 0)
        {
            messagesCount.latestMessagePosition = message.position;
        }

        // Stops at the first message already read (at or before afterPosition) and everything else is read too
        if (message.position <= afterPosition)
        {
            break;
        }

        // Counts unread message unless current user sent it
        if (message.senderId != excludedSenderId)
        {
            ++messagesCount.count;
        }
    }

    return messagesCount;
}

MessagesInMemoryRepository::MessagesCountsMap MessagesInMemoryRepository::CountByConversationIdsAfter(const AfterPositionsMap& afterPositionsMap, const std::string& excludedSenderId)
{
    MessagesCountsMap messagesCountsMap = {};
    messagesCountsMap.reserve(afterPositionsMap.size());

    /**
     * NOTES:
     *  - Gives every conversation an entry with count 0 (so a conversation without messages is still returned)
     *  - Finds smallest afterPosition (every message below it is read in every conversation so the walk can stop there)
     *  - Starts at max value so the first real position always replaces it
     */
    std::uint64_t smallestAfterPosition = std::numeric_limits<std::uint64_t>::max();
    for (const AfterPositionsMap::value_type& afterPositionsPair : afterPositionsMap)
    {
        messagesCountsMap[afterPositionsPair.first] = {};
        smallestAfterPosition = std::min(smallestAfterPosition, afterPositionsPair.second);
    }

    // NOTE: Walks m_messages once from newest to oldest and applies CountByConversationIdAfter() rules to each message's conversation
    for (std::vector<Message>::const_reverse_iterator messagesReverseIterator = m_messages.crbegin(); messagesReverseIterator != m_messages.crend(); ++messagesReverseIterator)
    {
        const Message& message = *messagesReverseIterator;

        /**
         * NOTES:
         *  - Stops at first message below smallestAfterPosition (it and every older messages are read in every conversation)
         *  - < and not <= so the message at smallestAfterPosition is still visited because it can be the newest message of
         *    its conversation (afterPosition is always a message of that conversation)
         */
        if (message.position < smallestAfterPosition)
        {
            break;
        }

        // Skips messages of conversations that were not asked for (conversation id of message is not in afterPositionsMap)
        const AfterPositionsMap::const_iterator afterPositionsMapIterator = afterPositionsMap.find(message.conversationId);
        if (afterPositionsMapIterator == afterPositionsMap.end())
        {
            continue;
        }

        MessagesCount& messagesCount = messagesCountsMap[message.conversationId];

        /**
         * NOTES:
         *  - Saves position of the first message met for this conversation (it is the newest one because the walk goes from newest to oldest)
         *  - Real messages never have position 0 because positions start at 1
         */
        if (messagesCount.latestMessagePosition == 0)
        {
            messagesCount.latestMessagePosition = message.position;
        }

        // Skips messages already read in this conversation (at or before afterPosition)
        if (message.position <= afterPositionsMapIterator->second)
        {
            continue;
        }

        // Counts unread message unless current user sent it
        if (message.senderId != excludedSenderId)
        {
            ++messagesCount.count;
        }
    }

    return messagesCountsMap;
}

Message MessagesInMemoryRepository::Create(const Message& message)
{
    // NOTE: Assigns position like a database sequence (the service never sets it)
    Message positionedMessage = message;
    positionedMessage.position = m_nextPosition;
    ++m_nextPosition;

    m_messages.push_back(positionedMessage);

    return positionedMessage;
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

std::optional<Message> MessagesInMemoryRepository::FindById(const std::string& id)
{
    // NOTE: Loops backward because lookups are almost always for the newest messages
    std::vector<Message>::const_reverse_iterator messagesReverseIterator = std::find_if(
        m_messages.crbegin(),
        m_messages.crend(),
        [&id](const Message& message) {
            bool isFound = message.id == id;

            return isFound;
        }
    );

    if (messagesReverseIterator == m_messages.crend())
    {
        return std::nullopt;
    }

    return *messagesReverseIterator;
}
