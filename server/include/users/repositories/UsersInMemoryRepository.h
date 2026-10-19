#pragma once

#include "models/User.h"
#include "users/repositories/IUsersRepository.h"

#include <optional>
#include <string>
#include <unordered_set>
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
    std::vector<User> FindByIds(const std::unordered_set<std::string>& ids) const override;
    std::vector<User> Search(const std::string& search) const override;

private:
    std::vector<User> m_users = {};

    // NOTE: Borrowed from the module, which owns it and outlives this repository
    Logger& m_logger;
};
