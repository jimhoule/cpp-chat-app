#include "messages/MessagesReadStates.h"

// **********
// * PUBLIC *
// **********
void MessagesReadStates::ApplyLiveMessage(const MessageView& message, bool isUnread)
{
    // NOTE: No entry means the server's read state is not here yet (it will include this message)
    MessagesReadStateView* messagesReadState = FindMutable(message.conversationId);
    if (messagesReadState == nullptr)
    {
        return;
    }

    // NOTE: Already counted by the server (the read state was built after this message was created)
    if (message.position <= messagesReadState->latestMessagePosition)
    {
        return;
    }

    // NOTE: Advanced even when not unread (own message is still the newest and a later read state must not count it)
    messagesReadState->latestMessagePosition = message.position;

    if (isUnread)
    {
        messagesReadState->unreadCount++;
    }
}

void MessagesReadStates::Clear()
{
    m_messagesReadStatesMap.clear();
}

const MessagesReadStateView* MessagesReadStates::Find(const std::string& conversationId) const
{
    const MessagesReadStatesMap::const_iterator messagesReadStatesMapIterator = m_messagesReadStatesMap.find(conversationId);
    if (messagesReadStatesMapIterator == m_messagesReadStatesMap.cend())
    {
        return nullptr;
    }

    return &messagesReadStatesMapIterator->second;
}

void MessagesReadStates::Remove(const std::string& conversationId)
{
    m_messagesReadStatesMap.erase(conversationId);
}

void MessagesReadStates::Set(const MessagesReadStateView& messagesReadState)
{
    m_messagesReadStatesMap[messagesReadState.conversationId] = messagesReadState;
}

void MessagesReadStates::SetAll(const std::vector<MessagesReadStateView>& messagesReadStates)
{
    Clear();

    for (const MessagesReadStateView& messagesReadState : messagesReadStates)
    {
        m_messagesReadStatesMap[messagesReadState.conversationId] = messagesReadState;
    }
}

// ***********
// * PRIVATE *
// ***********
MessagesReadStateView* MessagesReadStates::FindMutable(const std::string& conversationId)
{
    /**
     * NOTES:
     *  - Reuses Find() so the lookup logic lives in one place
     *  - Find() returns a const pointer so const_cast() removes the const to allow modifying the entry
     *  - Safe because the map itself is not const (only Find() hands it back as const)
     */
    return const_cast<MessagesReadStateView*>(Find(conversationId));
}
