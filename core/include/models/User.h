#pragma once

#include <string>

struct User
{
    User() = default;

    std::string id;
    std::string email;
    std::string firstName;
    std::string lastName;
    std::string password;
};
