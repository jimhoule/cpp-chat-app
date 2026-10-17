#include "conversations/ConversationsService.h"

#include "log/Logger.h"
#include "users/UsersService.h"
#include "uuid/UuidService.h"

#include <unordered_set>

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
    const std::optional<Conversation> conversation = m_conversationsRepository->UpdateIsOpen(closeConversationDto.conversationId, closeConversationDto.userId, false);
    if (!conversation.has_value())
    {
        conversationResult.code = ConversationsResultCode::UNKNOWN_CONVERSATION;
        return conversationResult;
    }

    const ConversationView conversationView = ConvertConversationToConversationView(conversation.value());
    conversationResult.data = conversationView;

    return conversationResult;
}

ConversationsService::ConversationResult ConversationsService::Open(const OpenConversationDto& openConversationDto)
{
    ConversationResult conversationResult = {};

    // Checks if users other than the initiator will be in the conversation
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

    // Checks if users exist
    const std::unordered_set<std::string> userIds(normalizedUserIds.begin(), normalizedUserIds.end());

    UsersService::FindUsersByIdsDto findUsersByIdsDto = {};
    findUsersByIdsDto.ids = userIds;
    const UsersService::UsersResult usersResult = m_usersService.FindByIds(findUsersByIdsDto);
    if (usersResult.data.size() != normalizedUserIds.size())
    {
        conversationResult.code = ConversationsResultCode::UNKNOWN_USER;
        return conversationResult;
    }

    // Returns conversation if exists so no creation (this is what makes Open() idempotent)
    const std::optional<Conversation> existingConversation = m_conversationsRepository->FindByUserIds(normalizedUserIds);
    if (existingConversation.has_value())
    {
        const std::optional<Conversation> openedConversation = m_conversationsRepository->UpdateIsOpen(existingConversation.value().id, openConversationDto.initiatorUserId, true);
        
        const ConversationView conversationView = ConvertConversationToConversationView(openedConversation.value());
        conversationResult.data = conversationView;

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

    const ConversationView conversationView = ConvertConversationToConversationView(m_conversationsRepository->Create(conversation));
    conversationResult.data = conversationView;

    return conversationResult;
}

ConversationsService::ConversationResult ConversationsService::OpenForAllUsers(const OpenConversationForAllUsersDto& openConversationForAllUsersDto)
{
    ConversationResult conversationResult = {};

    const std::optional<Conversation> conversation = m_conversationsRepository->FindById(openConversationForAllUsersDto.conversationId);
    if (!conversation.has_value())
    {
        conversationResult.code = ConversationsResultCode::UNKNOWN_CONVERSATION;
        return conversationResult;
    }

    /**
     * NOTES:
     *  - If every user is already open the loop will never assign an open conversation
     *  - So starting from conversation means it still returns the conversation which in
     *    that case has already everyone open
     */
    std::optional<Conversation> openedConversation = conversation;
    for (const ConversationUser& conversationUser : conversation.value().users)
    {
        // Skips if already open
        if (conversationUser.isOpen)
        {
            continue;
        }

        // Opens conversation
        openedConversation = m_conversationsRepository->UpdateIsOpen(conversation.value().id, conversationUser.userId, true);
    }

    const ConversationView conversationView = ConvertConversationToConversationView(openedConversation.value());
    conversationResult.data = conversationView;

    return conversationResult;
}

ConversationsService::ConversationResult ConversationsService::FindById(const FindConversationByIdDto& findConversationByIdDto)
{
    ConversationResult conversationResult = {};

    const std::optional<Conversation> conversation = m_conversationsRepository->FindById(findConversationByIdDto.id);
    if (!conversation.has_value())
    {
        return conversationResult;
    }

    const ConversationView conversationView = ConvertConversationToConversationView(conversation.value());
    conversationResult.data = conversationView;

    return conversationResult;
}

ConversationsService::ConversationResult ConversationsService::FindByUserIds(const FindConversationByUserIdsDto& findConversationByUserIdsDto)
{
    ConversationResult conversationResult = {};

    const std::vector<std::string> normalizedUserIds = NormalizeUserIds(findConversationByUserIdsDto.initiatorUserId, findConversationByUserIdsDto.userIds);
    const std::optional<Conversation> conversation = m_conversationsRepository->FindByUserIds(normalizedUserIds);
    if (!conversation.has_value())
    {
        return conversationResult;
    }

    const ConversationView conversationView = ConvertConversationToConversationView(conversation.value());
    conversationResult.data = conversationView;

    return conversationResult;
}

ConversationsService::ConversationsResult ConversationsService::FindAllOpenByUserId(const FindAllOpenConversationsByUserIdDto& findAllOpenConversationsByUserIdDto)
{
    // Fetches conversations
    const std::vector<Conversation> conversations = m_conversationsRepository->FindAllOpenByUserId(findAllOpenConversationsByUserIdDto.userId);

    // Builds conversation views
    const std::vector<ConversationView> conversationViews = ConvertConversationsToConversationViews(conversations);

    ConversationsResult conversationsResult = {};
    conversationsResult.data = conversationViews;

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
ConversationsService::UserViewsMap ConversationsService::BuildUserViewsMap(const std::vector<Conversation>& conversations) const
{
    // Collects user ids of all conversations
    std::unordered_set<std::string> userIds = {};
    for (const Conversation& conversation : conversations)
    {
        for (const ConversationUser& conversationUser : conversation.users)
        {
            userIds.insert(conversationUser.userId);
        }
    }

    // Fetches all user views in one batch
    UsersService::FindUsersByIdsDto findUsersByIdsDto = {};
    findUsersByIdsDto.ids = userIds;
    const UsersService::UsersResult usersResult = m_usersService.FindByIds(findUsersByIdsDto);

    // Indexes user views by id
    UserViewsMap userViewsMap = {};
    for (const UserView& userView : usersResult.data)
    {
        userViewsMap[userView.id] = userView;
    }

    return userViewsMap;
}

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

std::vector<ConversationView> ConversationsService::ConvertConversationsToConversationViews(const std::vector<Conversation>& conversations) const
{
    // Builds user views map
    const UserViewsMap userViewsMap = BuildUserViewsMap(conversations);

    // Builds conversation views
    std::vector<ConversationView> conversationViews = {};
    for (const Conversation& conversation : conversations)
    {
        const ConversationView conversationView = ConvertConversationToConversationView(conversation, userViewsMap);
        conversationViews.push_back(conversationView);
    }

    return conversationViews;
}

ConversationView ConversationsService::ConvertConversationToConversationView(const Conversation& conversation) const
{
    // Builds user views map
    const std::vector<Conversation> conversations = { conversation };
    const UserViewsMap userViewsMap = BuildUserViewsMap(conversations);

    // Builds conversation view
    const ConversationView conversationView = ConvertConversationToConversationView(conversation, userViewsMap);

    return conversationView;
}

ConversationView ConversationsService::ConvertConversationToConversationView(const Conversation& conversation, const UserViewsMap& userViewsMap) const
{
    // Builds conversation user views
    std::vector<ConversationUserView> conversationUserViews = {};
    for (const ConversationUser& conversationUser : conversation.users)
    {
        UserViewsMap::const_iterator userViewsMapIterator = userViewsMap.find(conversationUser.userId);
        if (userViewsMapIterator == userViewsMap.end())
        {
            m_logger.Warning("User " + conversationUser.userId + " of conversation " + conversation.id + " not found, skipped");
            continue;
        }

        ConversationUserView conversationUserView = {};
        conversationUserView.isOpen = conversationUser.isOpen;
        conversationUserView.user = userViewsMapIterator->second;

        conversationUserViews.push_back(conversationUserView);
    }

    // Builds conversation view
    ConversationView conversationView = {};
    conversationView.id = conversation.id;
    conversationView.createdAt = conversation.createdAt;
    conversationView.users = conversationUserViews;

    return conversationView;
}
