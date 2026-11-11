#include "messages/MessageHistories.h"

#include <algorithm>
#include <iterator>
#include <unordered_set>
#include <utility>

// **********
// * PUBLIC *
// **********
void MessageHistories::Add(const std::string& conversationId)
{
    MessageHistory messageHistory = {};
    messageHistory.isLoading = true;
    m_messageHistoriesMap[conversationId] = std::move(messageHistory);
}

void MessageHistories::AppendLiveMessage(const MessageView& message)
{
    /**
     * NOTES:
     *  - No entry means the history is not loaded yet
     *  - First page fetch will include this message
     */
    MessageHistory* messageHistory = FindMutable(message.conversationId);
    if (messageHistory == nullptr)
    {
        return;
    }

    messageHistory->messages.push_back(message);
}

void MessageHistories::Clear()
{
    m_messageHistoriesMap.clear();
}

const MessageHistories::MessageHistory* MessageHistories::Find(const std::string& conversationId) const
{
    MessageHistoriesMap::const_iterator messageHistoriesMapIterator = m_messageHistoriesMap.find(conversationId);
    if (messageHistoriesMapIterator == m_messageHistoriesMap.end())
    {
        return nullptr;
    }

    return &messageHistoriesMapIterator->second;
}

bool MessageHistories::Has(const std::string& conversationId) const
{
    // NOTE: Calling Find() and not FindMutable() because const functions can only call other const functions
    const MessageHistory* messageHistory = Find(conversationId);
    const bool hasMessageHistory = messageHistory != nullptr;

    return hasMessageHistory;
}

void MessageHistories::MergePage(const std::string& conversationId, const MessagesPageView& messagesPage)
{
    // NOTE: No entry means the conversation was closed while the page was in flight
    MessageHistory* messageHistory = FindMutable(conversationId);
    if (messageHistory == nullptr)
    {
        return;
    }

    std::vector<MessageView> pageMessages = messagesPage.items;

    // Drops messages already in the history (live messages that arrived while the page was in flight)
    std::unordered_set<std::string> messageIds = {};
    for (const MessageView& message : messageHistory->messages)
    {
        messageIds.insert(message.id);
    }

    // Moves the messages to keep to the front and returns where they end (the vector size does not change yet)
    std::vector<MessageView>::iterator keptMessagesEndIterator = std::remove_if(
        pageMessages.begin(),
        pageMessages.end(),
        [&messageIds](const MessageView& message) {
            return messageIds.count(message.id) > 0;
        }
    );
    // Erases the leftovers messages after the kept ones (one leftover per duplicated message)
    pageMessages.erase(keptMessagesEndIterator, pageMessages.end());

    /**
     * NOTES:
     *  - Pages are oldest first and older than everything in the history so they go in front as is
     *  - make_move_iterator moves each message instead of copying it
     *  - Using the range version of insert() (insert(where, first, last))
     *  - Starts inserting at the front before the first message (just like push_front()) of the vector
     *    and inserts page messages from first to last
     */
    messageHistory->messages.insert(
        messageHistory->messages.begin(),
        std::make_move_iterator(pageMessages.begin()),
        std::make_move_iterator(pageMessages.end())
    );
    messageHistory->nextCursor = messagesPage.nextCursor;
    messageHistory->isLoading = false;
}

void MessageHistories::Remove(const std::string& conversationId)
{
    m_messageHistoriesMap.erase(conversationId);
}

void MessageHistories::StartLoading(const std::string& conversationId)
{
    MessageHistory* messageHistory = FindMutable(conversationId);
    if (messageHistory == nullptr)
    {
        return;
    }

    messageHistory->isLoading = true;
}

// ***********
// * PRIVATE *
// ***********
MessageHistories::MessageHistory* MessageHistories::FindMutable(const std::string& conversationId)
{
    /**
     * NOTES:
     *  - Reuses Find() so the lookup logic lives in one place
     *  - Find() returns a const pointer so const_cast() removes the const to allow modifying the entry
     *  - Safe because the map itself is not const (only Find() hands it back as const)
     */
    MessageHistory* messageHistory = const_cast<MessageHistory*>(Find(conversationId));

    return messageHistory;
}
