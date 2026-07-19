#pragma once

#include "IUsersRepository.h"

#include <vector>

class UsersInMemoryRepository : public IUsersRepository
{
public:
    UsersInMemoryRepository() = default;

    User Create(const User& user) override;
    std::optional<User> FindByEmail(const std::string& email) const override;

private:
    std::vector<User> m_users = {};
};
