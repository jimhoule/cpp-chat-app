#pragma once

#include "views/MessageView.h"
#include "views/MessagesPageView.h"

#include <string>
#include <unordered_map>
#include <vector>
/**
 * NOTES:
 *  - Holds the loaded message history of each conversation keyed by conversation id
 *  - An entry means history is currently loading or already loaded (it is added right
 *    before first page fetch is sent)
 *  - Fetching stays in the caller (this class has no GUI or network dependencies)
 */
class MessageHistories
{
public:
    struct MessageHistory
    {
        std::vector<MessageView> messages;
        // NOTE: Sent back as the cursor when scrolling up (empty means no older messages)
        std::string nextCursor;
    };

    // NOTE: Adds empty entry (called right before first page fetch)
    void Add(const std::string& conversationId);
    // NOTE: Ignored without an entry because first page will include the message
    void AppendLiveMessage(const MessageView& message);
    void Clear();
    // NOTE: nullptr if history not currently loading or already loaded
    const MessageHistory* Find(const std::string& conversationId) const;
    // NOTE: True if history is currently loading or already loaded
    bool Has(const std::string& conversationId) const;
    // NOTE: Ignored without an entry (conversation closed while the page was in flight)
    void MergePage(const std::string& conversationId, const MessagesPageView& messagesPage);
    void Remove(const std::string& conversationId);

private:
    using MessageHistoriesMap = std::unordered_map<std::string, MessageHistory>;

    MessageHistoriesMap m_messageHistoriesMap = {};
};
