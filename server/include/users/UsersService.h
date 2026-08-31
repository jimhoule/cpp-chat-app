#pragma once

#include "results/ServiceResult.h"
#include "users/repositories/IUsersRepository.h"

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
        INVALID_PASSWORD
    };

    using UserResult = ServiceResult<UsersResultCode, std::optional<User>>;

    struct CreateUserDto
    {
        std::string email;
        std::string firstName;
        std::string lastName;
        std::string password;
    };

    struct FindUserByEmailDto
    {
        std::string email;
    };

    struct FindUserByIdDto
    {
        std::string id;
    };

    struct VerifyUserPasswordDto
    {
        std::string password;
        std::string hashedPassword;
    };

    UsersService(std::unique_ptr<IUsersRepository> usersRepository, EncryptionService& encryptionService, UuidService& uuidService, Logger& logger);

    UserResult Create(const CreateUserDto& createUserDto);
    UserResult FindByEmail(const FindUserByEmailDto& findUserByEmailDto);
    UserResult FindById(const FindUserByIdDto& findUserByIdDto);
    UserResult VerifyPassword(const VerifyUserPasswordDto& verifyUserPasswordDto);
    std::string ConvertUsersResultCodeToString(UsersResultCode usersResultCode);

private:
    std::unique_ptr<IUsersRepository> m_usersRepository = nullptr;

    EncryptionService& m_encryptionService;
    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;
};
