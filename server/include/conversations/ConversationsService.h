#pragma once

#include "conversations/repositories/IConversationsRepository.h"

#include "models/Conversation.h"
#include "models/ConversationUser.h"
#include "results/ServiceResult.h"

#include <memory>
#include <optional>

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

    using ConversationResult = ServiceResult<ConversationsResultCode, std::optional<Conversation>>;
    using ConversationsResult = ServiceResult<ConversationsResultCode, std::vector<Conversation>>;

    struct CloseConversationDto
    {
        std::string conversationId;
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
        std::string conversationId;
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

    struct FindAllOpenConversationsByUserIdDto
    {
        std::string userId;
    };

    struct IsConversationUserDto
    {
        std::string conversationId;
        std::string userId;
    };

    ConversationsService(std::unique_ptr<IConversationsRepository> conversationsRepository, UsersService& usersService, UuidService& uuidService, Logger& logger);

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
     *  - Returns only the user ids that changed, so the caller knows who to notify
     */
    ConversationResult OpenForAllUsers(const OpenConversationForAllUsersDto& openConversationForAllUsersDto);
    ConversationResult FindById(const FindConversationByIdDto& findConversationByIdDto);
    // NOTE: Never creates anything, this is the lookup the client uses while picking the users of a new conversation
    ConversationResult FindByUserIds(const FindConversationByUserIdsDto& findConversationByUserIdsDto);
    ConversationsResult FindAllOpenByUserId(const FindAllOpenConversationsByUserIdDto& findAllOpenConversationsByUserIdDto);
    bool IsConversationUser(const IsConversationUserDto& isConversationUserDto);
    std::string ConvertConversationsResultCodeToString(ConversationsResultCode conversationsResultCode);

private:
    std::unique_ptr<IConversationsRepository> m_conversationsRepository = nullptr;

    UsersService& m_usersService;
    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;

    // NOTE: Adds initiator user id to user ids, sorts and deduplicates, every method taking user ids goes through it
    std::vector<std::string> NormalizeUserIds(const std::string& initiatorUserId, const std::vector<std::string>& userIds) const;
};
