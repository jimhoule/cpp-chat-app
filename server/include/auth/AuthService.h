#pragma once

#include "sessions/SessionsService.h"
#include "users/UsersService.h"

// Forward declarations
class Logger;

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
    AuthService(SessionsService& sessionsService, UsersService& usersService, Logger& logger);

    AuthServiceResult Login(const LoginDto& loginDto);
    AuthServiceResult Register(const RegisterDto& registerDto);

private:
    // NOTE: Here services must be a references because they are "borrowed" from other modules
    SessionsService& m_sessionsService;
    UsersService& m_usersService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;

    Session CreateSession(const std::string& userId);
};
