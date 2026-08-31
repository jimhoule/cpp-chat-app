#include "conversations/ConversationsService.h"

#include "users/UsersService.h"
#include "uuid/UuidService.h"

// NOTE: A conversation is identified by its exact set of users, so users can never be added to an existing one. Past this size that model breaks down and a named channel is needed
constexpr size_t MAX_CONVERSATION_USERS = 9;

// **********
// * PUBLIC *
// **********
ConversationsService::ConversationsService(std::unique_ptr<IConversationsRepository> conversationsRepository, UsersService& usersService, UuidService& uuidService, Logger& logger)
    : m_conversationsRepository(std::move(conversationsRepository))
    , m_usersService(usersService)
    , m_uuidService(uuidService)
    , m_logger(logger)
{}

ConversationsService::ConversationResult ConversationsService::Close(const CloseConversationDto& closeConversationDto)
{
    ConversationResult conversationResult = {};

    // Closes conversation
    std::optional<Conversation> conversation = m_conversationsRepository->SetIsOpen(closeConversationDto.conversationId, closeConversationDto.userId, false);
    if (!conversation.has_value())
    {
        conversationResult.code = ConversationsResultCode::UNKNOWN_CONVERSATION;
        return conversationResult;
    }

    conversationResult.data = conversation;

    return conversationResult;
}

ConversationsService::ConversationResult ConversationsService::Open(const OpenConversationDto& openConversationDto)
{
    ConversationResult conversationResult = {};

    // Checks if users other than the initator will be in the conversation
    if (openConversationDto.userIds.empty())
    {
        conversationResult.code = ConversationsResultCode::USERS_EMPTY;
        return conversationResult;
    }

    const std::vector<std::string> normalizedUserIds = NormalizeUserIds(openConversationDto.initiatorUserId, openConversationDto.userIds);

    // Checks users max count
    if (normalizedUserIds.size() > MAX_CONVERSATION_USERS)
    {
        conversationResult.code = ConversationsResultCode::TOO_MANY_USERS;
        return conversationResult;
    }

    // Checks if other users exist
    for (const std::string& normalizedUserId : normalizedUserIds)
    {
        // Skips initiator as he is already autthenticated
        if (normalizedUserId == openConversationDto.initiatorUserId)
        {
            continue;
        }

        // Checks if user exists
        UsersService::FindUserByIdDto findUserByIdDto = {};
        findUserByIdDto.id = normalizedUserId;
        UsersService::UserResult userResult = m_usersService.FindById(findUserByIdDto);
        if (!userResult.data.has_value())
        {
            conversationResult.code = ConversationsResultCode::UNKNOWN_USER;
            return conversationResult;
        }
    }

    // Returns conversation if exists so no creation (this is what makes Open() idempotent)
    std::optional<Conversation> existingConversation = m_conversationsRepository->FindByUserIds(normalizedUserIds);
    if (existingConversation.has_value())
    {
        m_conversationsRepository->SetIsOpen(existingConversation.value().id, openConversationDto.initiatorUserId, true);
        conversationResult.data = existingConversation;
        return conversationResult;
    }

    // Creates conversation if does not exist already
    Conversation conversation = {};
    conversation.id = m_uuidService.Generate();
    conversation.createdAt = std::time(0);
    for (const std::string& normalizedUserId : normalizedUserIds)
    {
        ConversationUser conversationUser = {};
        conversationUser.userId = normalizedUserId;
        conversationUser.isOpen = normalizedUserId == openConversationDto.initiatorUserId;

        conversation.users.push_back(conversationUser);
    }

    
    conversationResult.data = m_conversationsRepository->Create(conversation);

    return conversationResult;
}

ConversationsService::ConversationResult ConversationsService::OpenForAllUsers(const OpenConversationForAllUsersDto& openConversationForAllUsersDto)
{
    ConversationResult conversationResult = {};

    std::optional<Conversation> conversation = m_conversationsRepository->FindById(openConversationForAllUsersDto.conversationId);
    if (!conversation.has_value())
    {
        conversationResult.code = ConversationsResultCode::UNKNOWN_CONVERSATION;
        return  conversationResult;
    }

    for (const ConversationUser& conversationUser : conversation.value().users)
    {
        // Skips if already open
        if (conversationUser.isOpen)
        {
            continue;
        }

        // Opens conversation
        const std::optional<Conversation> openConversation = m_conversationsRepository->SetIsOpen(conversation.value().id, conversationUser.userId, true);
        if (openConversation.has_value())
        {
            conversationResult.data = openConversation;
        }
    }

    return conversationResult;
}

ConversationsService::ConversationResult ConversationsService::FindById(const FindConversationByIdDto& findConversationByIdDto)
{
    ConversationResult conversationResult = {};
    conversationResult.data = m_conversationsRepository->FindById(findConversationByIdDto.id);

    return conversationResult;
}

ConversationsService::ConversationResult ConversationsService::FindByUserIds(const FindConversationByUserIdsDto& findConversationByUserIdsDto)
{
    const std::vector<std::string> normalizedUserIds = NormalizeUserIds(findConversationByUserIdsDto.initiatorUserId, findConversationByUserIdsDto.userIds);

    ConversationResult conversationResult = {};
    conversationResult.data = m_conversationsRepository->FindByUserIds(normalizedUserIds);

    return conversationResult;
}

ConversationsService::ConversationsResult ConversationsService::FindAllOpenByUserId(const FindAllOpenConversationsByUserIdDto& findAllOpenConversationsByUserIdDto)
{
    ConversationsResult conversationsResult = {};
    conversationsResult.data = m_conversationsRepository->FindAllOpenByUserId(findAllOpenConversationsByUserIdDto.userId);

    return conversationsResult;
}

bool ConversationsService::IsConversationUser(const IsConversationUserDto& isConversationUserDto)
{
    return m_conversationsRepository->IsConversationUser(isConversationUserDto.conversationId, isConversationUserDto.userId);
}

std::string ConversationsService::ConvertConversationsResultCodeToString(ConversationsResultCode conversationsResultCode)
{
    switch (conversationsResultCode)
    {
        case ConversationsResultCode::OK:
            return "OK";

        case ConversationsResultCode::USERS_EMPTY:
            return "USERS EMPTY";

        case ConversationsResultCode::TOO_MANY_USERS:
            return "TOO MANY USERS";

        case ConversationsResultCode::UNKNOWN_USER:
            return "UNKNOWN USER";

        case ConversationsResultCode::UNKNOWN_CONVERSATION:
            return "UNKNOWN CONVERSATION";

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return "Unknown conversations result code";
    }
}

// ***********
// * PRIVATE *
// ***********
std::vector<std::string> ConversationsService::NormalizeUserIds(const std::string& initiatorUserId, const std::vector<std::string>& userIds) const
{
    std::vector<std::string> normalizedUserIds = {};
    normalizedUserIds.reserve(userIds.size() + 1);

    // Keeps only the non empty user ids, an empty string is never a valid user id
    for (const std::string& userId : userIds)
    {
        if (!userId.empty())
        {
            normalizedUserIds.push_back(userId);
        }
    }

    // NOTE: Always added, the duplicate is removed below if the client also sent it
    normalizedUserIds.push_back(initiatorUserId);

    // Sorts and deduplicates so the same set of users always produces the same vector
    std::sort(normalizedUserIds.begin(), normalizedUserIds.end());
    normalizedUserIds.erase(std::unique(normalizedUserIds.begin(), normalizedUserIds.end()), normalizedUserIds.end());

    return normalizedUserIds;
}
