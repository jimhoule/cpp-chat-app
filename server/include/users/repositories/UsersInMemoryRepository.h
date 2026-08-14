#pragma once

#include "IUsersRepository.h"

#include <vector>

// Forward declarations
class Logger;

class UsersInMemoryRepository : public IUsersRepository
{
public:
    UsersInMemoryRepository(Logger& logger);

    User Create(const User& user) override;
    std::optional<User> FindByEmail(const std::string& email) const override;
    std::optional<User> FindById(const std::string& id) const override;

private:
    std::vector<User> m_users = {};

    // NOTE: Borrowed from the module, which owns it and outlives this repository
    Logger& m_logger;
};
