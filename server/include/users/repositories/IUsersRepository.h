#pragma once

#include "models/User.h"

#include "optional"
#include <string>

class IUsersRepository
{
public:
    // NOTE: Explicitly default the constructor (Optional but recommended for clarity)
    IUsersRepository() = default;

    // NOTE: ALWAYS provide a public virtual destructor
    virtual ~IUsersRepository() = default;

    // Disables copying and moving to prevent slicing
    IUsersRepository(const IUsersRepository&) = delete;
    IUsersRepository& operator=(const IUsersRepository&) = delete;

    virtual User Create(const User& user) = 0;
    virtual std::optional<User> FindByEmail(const std::string& email) const = 0;
};
