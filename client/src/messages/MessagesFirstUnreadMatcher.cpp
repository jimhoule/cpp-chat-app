#include "messages/MessagesFirstUnreadMatcher.h"

// **********
// * PUBLIC *
// **********
void MessagesFirstUnreadMatcher::Capture(const MessagesReadStateView* messagesReadState)
{
    if (messagesReadState == nullptr || m_lastReadMessagePosition.has_value())
    {
        return;
    }

    m_lastReadMessagePosition = messagesReadState->lastReadMessagePosition;
}

void MessagesFirstUnreadMatcher::EndFrame()
{
    m_isMatched = false;
}

bool MessagesFirstUnreadMatcher::Match(const MessageView& message, const std::string& currentUserId)
{
    if (m_isMatched || !m_lastReadMessagePosition.has_value())
    {
        return false;
    }

    if (message.position <= m_lastReadMessagePosition.value() || message.senderId == currentUserId)
    {
        return false;
    }

    m_isMatched = true;

    return true;
}

void MessagesFirstUnreadMatcher::Reset()
{
    m_isMatched = false;
    m_lastReadMessagePosition.reset();
}
