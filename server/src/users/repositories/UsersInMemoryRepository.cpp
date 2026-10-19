#include "users/repositories/UsersInMemoryRepository.h"

#include "utils/Strings.h"

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
        if (user.email == email)
        {
            return user;
        }
    }

    return std::nullopt;
}

std::optional<User> UsersInMemoryRepository::FindById(const std::string& id) const
{
    for (const User& user : m_users)
    {
        if (user.id == id)
        {
            return user;
        }
    }

    return std::nullopt;
}

std::vector<User> UsersInMemoryRepository::FindByIds(const std::unordered_set<std::string>& ids) const
{
    std::vector<User> users = {};
    for (const User& user : m_users)
    {
        bool isFound = ids.find(user.id) != ids.end();
        if (isFound)
        {
            users.push_back(user);
        }
    }

    return users;
}

std::vector<User> UsersInMemoryRepository::Search(const std::string& search) const
{
    const std::string lowerCaseSearch = ConvertStringToLowerCase(search);

    std::vector<User> users = {};
    for (const User& user : m_users)
    {
        const std::string lowerCaseEmail = ConvertStringToLowerCase(user.email);
        const std::size_t emailPos = lowerCaseEmail.find(lowerCaseSearch);
        const bool isEmailFound = emailPos != std::string::npos;
        if (isEmailFound)
        {
            users.push_back(user);
            continue;
        }

        const std::string lowerCaseFirstName = ConvertStringToLowerCase(user.firstName);
        const std::size_t firstNamePos = lowerCaseFirstName.find(lowerCaseSearch);
        const bool isFirstNameFound = firstNamePos != std::string::npos;
        if (isFirstNameFound)
        {
            users.push_back(user);
            continue;
        }

        const std::string lowerCaseLastName = ConvertStringToLowerCase(user.lastName);
        const std::size_t lastNamePos = lowerCaseLastName.find(lowerCaseSearch);
        const bool isLastNameFound = lastNamePos != std::string::npos;
        if (isLastNameFound)
        {
            users.push_back(user);
        }
    }

    return users;
}
