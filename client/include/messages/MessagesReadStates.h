#pragma once

#include "views/MessageView.h"
#include "views/MessagesReadStateView.h"

#include <string>
#include <unordered_map>
#include <vector>

/**
 * NOTES:
 *  - Always set from the server's read states (unreadCount is never reset locally)
 *  - No entry means no read state yet (before login's read states arrive or an opened conversation
 *    or reopened conversation waiting for its read state)
 */
class MessagesReadStates
{
public:
    /**
     * NOTES:
     *  - Ignored without an entry (the server's read state will include the message)
     *  - Ignored if already counted by the server (message.position <= latestMessagePosition)
     *  - isUnread is decided by the caller (message not sent by current user and conversation not selected and loaded)
     */
    void ApplyLiveMessage(const MessageView& message, bool isUnread);
    void Clear();
    // NOTE: nullptr if no read state yet
    const MessagesReadStateView* Find(const std::string& conversationId) const;
    void Remove(const std::string& conversationId);
    // NOTE: Adds or overwrites a read state
    void Set(const MessagesReadStateView& messagesReadState);
    // NOTE: Sets every read state to this list dropping any other entry
    void SetAll(const std::vector<MessagesReadStateView>& messagesReadStates);

private:
    using MessagesReadStatesMap = std::unordered_map<std::string, MessagesReadStateView>;

    MessagesReadStatesMap m_messagesReadStatesMap = {};

    MessagesReadStateView* FindMutable(const std::string& conversationId);
};
