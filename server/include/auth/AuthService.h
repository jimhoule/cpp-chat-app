#pragma once

#include "users/UsersService.h"

struct AuthServiceResult
{
    std::string sessionId;
    std::optional<User> user;
};

struct LoginDto
{
    std::string email;
    std::string password;
};

struct RegisterDto
{
    std::string email;
    std::string firstName;
    std::string lastName;
    std::string password;
};

class AuthService
{
public:
    AuthService(UsersService& usersService);

    AuthServiceResult Login(const LoginDto& loginDto);
    AuthServiceResult Register(const RegisterDto& registerDto);

private:
    // NOTE: Here the service must be a reference because it is "borrowing" users service owned by users module
    UsersService& m_usersService;
};
