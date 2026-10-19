#pragma once

#include "results/ServiceResult.h"
#include "users/repositories/IUsersRepository.h"
#include "views/UserView.h"

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

// Forward declarations
class EncryptionService;
class Logger;
class UuidService;

class UsersService
{
public:
    enum class UsersResultCode
    {
        OK,
        EMAIL_ALREADY_USED,
        INVALID_PASSWORD,
        UNKNOWN_USER
    };

    using UserResult = ServiceResult<UsersResultCode, std::optional<UserView>>;
    using UsersResult = ServiceResult<UsersResultCode, std::vector<UserView>>;

    struct CreateUserDto
    {
        std::string email;
        std::string firstName;
        std::string lastName;
        std::string password;
    };

    struct FindUserByIdDto
    {
        std::string id;
    };

    struct FindUsersByIdsDto
    {
        std::unordered_set<std::string> ids;
    };

    struct SearchUsersDto
    {
        std::string search;
    };

    struct VerifyUserCredentialsDto
    {
        std::string email;
        std::string password;
    };

    UsersService(std::unique_ptr<IUsersRepository> usersRepository, EncryptionService& encryptionService, UuidService& uuidService, Logger& logger);

    UserResult Create(const CreateUserDto& createUserDto);
    UserResult FindById(const FindUserByIdDto& findUserByIdDto);
    UsersResult FindByIds(const FindUsersByIdsDto& findUsersByIdsDto);
    UsersResult Search(const SearchUsersDto& searchUsersDto);
    UserResult VerifyCredentials(const VerifyUserCredentialsDto& verifyUserCredentialsDto);
    std::string ConvertUsersResultCodeToString(UsersResultCode usersResultCode);

private:
    std::unique_ptr<IUsersRepository> m_usersRepository = nullptr;

    EncryptionService& m_encryptionService;
    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;

    UserView ConvertUserToUserView(const User& user) const;
};
