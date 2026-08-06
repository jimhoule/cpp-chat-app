#pragma once

#include "results/ServiceResult.h"
#include "users/repositories/IUsersRepository.h"

// Forward declarations
class Logger;
class UuidService;

enum class UsersResultCode
{
    OK,
    EMAIL_ALREADY_USED
};

std::string ConvertUsersResultCodeToString(UsersResultCode usersResultCode);

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

class UsersService
{
public:
    UsersService(std::unique_ptr<IUsersRepository> usersRepository, UuidService& uuidService, Logger& logger);

    UserResult Create(const CreateUserDto& createUserDto);
    UserResult FindByEmail(const FindUserByEmailDto& findUserByEmailDto);

private:
    std::unique_ptr<IUsersRepository> m_usersRepository = nullptr;

    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;
};
