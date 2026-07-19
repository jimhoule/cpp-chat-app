#include "users/UsersService.h"

// **********
// * PUBLIC *
// **********
UsersService::UsersService(std::unique_ptr<IUsersRepository> usersRepository) : m_usersRepository(std::move(usersRepository))
{}

User UsersService::Create(const CreateUserDto& createUserDto)
{
    User user = {};
    user.id = "uuid." + createUserDto.email;
    user.email = createUserDto.email;
    user.firstName = createUserDto.firstName;
    user.lastName = createUserDto.lastName;
    user.password = "hashed." + createUserDto.password;


    return m_usersRepository->Create(user);
}

std::optional<User> UsersService::FindByEmail(const FindUserByEmailDto& findUserByEmailDto)
{
    return m_usersRepository->FindByEmail(findUserByEmailDto.email);
}