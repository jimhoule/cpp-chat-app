#pragma once

#include "users/repositories/IUsersRepository.h"

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
    UsersService(std::unique_ptr<IUsersRepository> usersRepository);

    User Create(const CreateUserDto& createUserDto);
    std::optional<User> FindByEmail(const FindUserByEmailDto& findUserByEmailDto);

private:
    std::unique_ptr<IUsersRepository> m_usersRepository = nullptr;
};
