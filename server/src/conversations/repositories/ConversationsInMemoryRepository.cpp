#include "conversations/repositories/ConversationsInMemoryRepository.h"

#include "log/Logger.h"

// **********
// * PUBLIC *
// **********
ConversationsInMemoryRepository::ConversationsInMemoryRepository(Logger& logger)
    : m_logger(logger)
{}

std::optional<Conversation> ConversationsInMemoryRepository::AdvanceLastReadMessagePosition(
    const std::string& id,
    const std::string& userId,
    std::uint64_t messagePosition
)
{
    return UpdateConversationUser(
        id,
        userId,
        [messagePosition](ConversationUser& conversationUser) {
            if (messagePosition > conversationUser.lastReadMessagePosition)
            {
                conversationUser.lastReadMessagePosition = messagePosition;
            }
        }
    );
}

Conversation ConversationsInMemoryRepository::Create(const Conversation& conversation)
{
    m_conversations.push_back(conversation);

    return conversation;
}

std::vector<Conversation> ConversationsInMemoryRepository::FindAllOpenByUserId(const std::string& userId)
{
    std::vector<Conversation> openConversations = {};
    for (Conversation& conversation : m_conversations)
    {
        const ConversationUser* conversationUser = FindConversationUserPointer(conversation, userId);
        if (conversationUser == nullptr)
        {
            continue;
        }

        if (!conversationUser->isOpen)
        {
            continue;
        }

        openConversations.push_back(conversation);
    }

    return openConversations;
}

ConversationsInMemoryRepository::LastReadMessagePositionsMap ConversationsInMemoryRepository::FindAllOpenLastReadMessagePositionsByUserId(const std::string& userId)
{
    LastReadMessagePositionsMap lastReadMessagePositionsMap = {};
    for (Conversation& conversation : m_conversations)
    {
        const ConversationUser* conversationUser = FindConversationUserPointer(conversation, userId);
        if (conversationUser == nullptr)
        {
            continue;
        }

        if (!conversationUser->isOpen)
        {
            continue;
        }

        lastReadMessagePositionsMap[conversation.id] = conversationUser->lastReadMessagePosition;
    }

    return lastReadMessagePositionsMap;
}

std::optional<Conversation> ConversationsInMemoryRepository::FindById(const std::string& id)
{
    Conversation* conversation = FindConversationPointer(id);
    if (conversation == nullptr)
    {
        return std::nullopt;
    }

    // NOTE: Copies on purpose, callers outside this repository must not hold a pointer into m_conversations
    return *conversation;
}

std::optional<Conversation> ConversationsInMemoryRepository::FindByUserIds(const std::vector<std::string>& userIds)
{
    for (Conversation& conversation : m_conversations)
    {
        // Compares vectors lengths (rejects most conversations in constant time before any user is compared)
        if (conversation.users.size() != userIds.size())
        {
            continue;
        }

        // Checks if all users are the same
        bool hasSameUsers = true;
        for (const std::string& userId : userIds)
        {
            ConversationUser* conversationUser = FindConversationUserPointer(conversation, userId);
            if (conversationUser == nullptr)
            {
                hasSameUsers = false;
                break;
            }
        }

        if (!hasSameUsers)
        {
            continue;
        }

        // NOTE: Copies on purpose, callers outside this repository must not hold a pointer into m_conversations
        return conversation;
    }

    return std::nullopt;
}

std::optional<std::uint64_t> ConversationsInMemoryRepository::FindLastReadMessagePositionById(const std::string& id, const std::string& userId)
{
    Conversation* conversation = FindConversationPointer(id);
    if (conversation == nullptr)
    {
        return std::nullopt;
    }

    ConversationUser* conversationUser = FindConversationUserPointer(*conversation, userId);
    if (conversationUser == nullptr)
    {
        return std::nullopt;
    }

    return conversationUser->lastReadMessagePosition;
}

bool ConversationsInMemoryRepository::IsConversationUser(const std::string& id, const std::string& userId)
{
    Conversation* conversation = FindConversationPointer(id);
    if (conversation == nullptr)
    {
        return false;
    }

    ConversationUser* conversationUser = FindConversationUserPointer(*conversation, userId);

    return conversationUser != nullptr;
}

std::optional<Conversation> ConversationsInMemoryRepository::UpdateIsOpen(const std::string& id, const std::string& userId, bool isOpen)
{
    return UpdateConversationUser(
        id,
        userId,
        [isOpen](ConversationUser& conversationUser) {
            conversationUser.isOpen = isOpen;
        }
    );
}

// ***********
// * PRIVATE *
// ***********
Conversation* ConversationsInMemoryRepository::FindConversationPointer(const std::string& id)
{
    for (Conversation& conversation : m_conversations)
    {
        if (conversation.id == id)
        {
            return &conversation;
        }
    }

    return nullptr;
}

  ConversationUser* ConversationsInMemoryRepository::FindConversationUserPointer(Conversation& conversation, const std::string& userId)
  {
    for (ConversationUser& conversationUser : conversation.users)
    {
        if (conversationUser.userId == userId)
        {
            return &conversationUser;
        }
    }

    return nullptr;
}

std::optional<Conversation> ConversationsInMemoryRepository::UpdateConversationUser(
    const std::string& id,
    const std::string& userId,
    const std::function<void(ConversationUser&)>& update
)
{
    Conversation* conversation = FindConversationPointer(id);
    if (conversation == nullptr)
    {
        m_logger.Error("Conversation " + id + " not found");
        return std::nullopt;
    }

    ConversationUser* conversationUser = FindConversationUserPointer(*conversation, userId);
    if (conversationUser == nullptr)
    {
        m_logger.Error("User " + userId + " is not in conversation " + id);
        return std::nullopt;
    }

    // NOTE: conversationUser points into m_conversations, so this updates the stored conversation
    update(*conversationUser);

    // NOTE: Copies on purpose, callers outside this repository must not hold a pointer into m_conversations
    return *conversation;
}
