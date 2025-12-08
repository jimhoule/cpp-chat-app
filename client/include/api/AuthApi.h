#pragma once

#include <string>

struct LoginParams
{
    std::string Email;
    std::string Password;

};

struct RegisterParams
{
    std::string FirstName;
    std::string LastName;
    std::string Email;
    std::string Password;

};

class AuthApi
{
public:
    AuthApi() = default;

    void Login(const LoginParams& LoginParams) const;
    void Register(const RegisterParams& RegisterParams) const;
};
