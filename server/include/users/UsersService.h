#pragma once

#include "users/repositories/IUsersRepository.h"

// Forward declarations
class Logger;

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
    UsersService(std::unique_ptr<IUsersRepository> usersRepository, Logger& logger);

    User Create(const CreateUserDto& createUserDto);
    std::optional<User> FindByEmail(const FindUserByEmailDto& findUserByEmailDto);

private:
    std::unique_ptr<IUsersRepository> m_usersRepository = nullptr;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;
};
