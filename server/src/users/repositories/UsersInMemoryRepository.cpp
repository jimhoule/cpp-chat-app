#include "users/repositories/UsersInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
UsersInMemoryRepository::UsersInMemoryRepository(Logger& logger)
    : m_logger(logger)
{}

User UsersInMemoryRepository::Create(const User& user)
{
    m_users.push_back(user);

    return user;
}

std::optional<User> UsersInMemoryRepository::FindByEmail(const std::string& email) const
{
    for (const User& user : m_users)
    {
        if (user.email == email) return user;
    }

    return std::nullopt;
}