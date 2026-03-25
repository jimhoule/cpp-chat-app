#pragma once

#include <string>

struct User
{
    User() = default;

    std::string ID;
    std::string FirstName;
    std::string LastName;
    std::string ImageUrl;
};
