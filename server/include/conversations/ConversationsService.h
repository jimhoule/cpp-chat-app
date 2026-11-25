#pragma once

#include "conversations/repositories/IConversationsRepository.h"

#include "models/Conversation.h"
#include "results/ServiceResult.h"
#include "views/ConversationView.h"
#include "views/UserView.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class Logger;
class UsersService;
class UuidService;

class ConversationsService
{
public:
    enum class ConversationsResultCode
    {
        OK,
        USERS_EMPTY,
        TOO_MANY_USERS,
        UNKNOWN_USER,
        UNKNOWN_CONVERSATION
    };

    using ConversationResult = ServiceResult<ConversationsResultCode, std::optional<ConversationView>>;
    using ConversationsResult = ServiceResult<ConversationsResultCode, std::vector<ConversationView>>;
    using LastReadMessagePositionResult = ServiceResult<ConversationsResultCode, std::uint64_t>;
    using LastReadMessagePositionsResult = ServiceResult<ConversationsResultCode, IConversationsRepository::LastReadMessagePositionsMap>;

    struct AdvanceLastReadMessagePositionDto
    {
        std::string id;
        std::string userId;
        std::uint64_t messagePosition;
    };

    struct CloseConversationDto
    {
        std::string id;
        std::string userId;
    };

    struct OpenConversationDto
    {
        std::string initiatorUserId;
        // NOTE: The other users, the initiator is added and does not have to be in it
        std::vector<std::string> userIds;
    };

    struct OpenConversationForAllUsersDto
    {
        std::string id;
    };

    struct FindAllOpenConversationsByUserIdDto
    {
        std::string userId;
    };

    struct FindAllOpenLastReadMessagePositionsByUserIdDto
    {
        std::string userId;
    };

    struct FindConversationByIdDto
    {
        std::string id;
    };

    struct FindConversationByUserIdsDto
    {
        // NOTE: Always added to userIds, a user can only look up conversations they belong to
        std::string initiatorUserId;
        std::vector<std::string> userIds;
    };

    struct FindLastReadMessagePositionByIdDto
    {
        std::string id;
        std::string userId;
    };

    struct IsConversationUserDto
    {
        std::string id;
        std::string userId;
    };

    ConversationsService(std::unique_ptr<IConversationsRepository> conversationsRepository, UsersService& usersService, UuidService& uuidService, Logger& logger);

    LastReadMessagePositionResult AdvanceLastReadMessagePosition(const AdvanceLastReadMessagePositionDto& advanceLastReadMessagePositionDto);
    ConversationResult Close(const CloseConversationDto& closeConversationDto);
    /**
     * NOTES:
     *  - Returns the conversation for this exact set of users, creating it if none exists yet
     *  - Makes it open for the initiator, the other users stay closed until the first message is sent
     *  - Idempotent, calling it twice for the same users returns the same conversation
     */
    ConversationResult Open(const OpenConversationDto& openConversationDto);
    /**
     * NOTES:
     *  - Opens the conversation for every user who still has it closed, called when a message is sent
     *  - Returns conversation with every user open whether or not anything changed
     */
    ConversationResult OpenForAllUsers(const OpenConversationForAllUsersDto& openConversationForAllUsersDto);
    ConversationsResult FindAllOpenByUserId(const FindAllOpenConversationsByUserIdDto& findAllOpenConversationsByUserIdDto);
    /**
     * NOTES:
     *  - Returns the user's lastReadMessagePosition for every open conversation
     *  - Used at login to build read states
     *  - It does not build any views (ConversationUserView leaves positions out so other participants can't
     *    see who read what)
     */
    LastReadMessagePositionsResult FindAllOpenLastReadMessagePositionsByUserId(const FindAllOpenLastReadMessagePositionsByUserIdDto& findAllOpenLastReadMessagePositionsByUserIdDto);
    ConversationResult FindById(const FindConversationByIdDto& findConversationByIdDto);
    // NOTE: Never creates anything, this is the lookup the client uses while picking the users of a new conversation
    ConversationResult FindByUserIds(const FindConversationByUserIdsDto& findConversationByUserIdsDto);
    /**
     * NOTES:
     *  - Returns the user's lastReadMessagePosition in this conversation (no view)
     *  - UNKNOWN_CONVERSATION if the conversation is not found or the user is not in it
     */
    LastReadMessagePositionResult FindLastReadMessagePositionById(const FindLastReadMessagePositionByIdDto& findLastReadMessagePositionByIdDto);
    bool IsConversationUser(const IsConversationUserDto& isConversationUserDto);
    std::string ConvertConversationsResultCodeToString(ConversationsResultCode conversationsResultCode);

private:
    using UserViewsMap = std::unordered_map<std::string, UserView>;

    std::unique_ptr<IConversationsRepository> m_conversationsRepository = nullptr;

    UsersService& m_usersService;
    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;

    UserViewsMap BuildUserViewsMap(const std::vector<Conversation>& conversations) const;
    std::vector<ConversationView> ConvertConversationsToConversationViews(const std::vector<Conversation>& conversations) const;
    ConversationView ConvertConversationToConversationView(const Conversation& conversation) const;
    ConversationView ConvertConversationToConversationView(const Conversation& conversation, const UserViewsMap& userViewsMap) const;
    // NOTE: Adds initiator user id to user ids, sorts and deduplicates, every method taking user ids goes through it
    std::vector<std::string> NormalizeUserIds(const std::string& initiatorUserId, const std::vector<std::string>& userIds) const;
};
