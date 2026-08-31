#pragma once

#include "results/ServiceResult.h"
#include "sessions/SessionsService.h"
#include "users/UsersService.h"

// Forward declarations
class Logger;

class AuthService
{
public:
    enum class AuthResultCode
    {
        OK,
        USER_NOT_FOUND,
        INVALID_PASSWORD,
        EMAIL_ALREADY_USED
    };

    struct Auth
    {
        std::string sessionId = "";
        User user = {};
    };
    using AuthResult = ServiceResult<AuthResultCode, Auth>;

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

    AuthService(SessionsService& sessionsService, UsersService& usersService, Logger& logger);

    AuthResult Login(const LoginDto& loginDto);
    AuthResult Register(const RegisterDto& registerDto);
    std::string ConvertAuthResultCodeToString(AuthResultCode authResultCode);

private:
    // NOTE: Here services must be a references because they are "borrowed" from other modules
    SessionsService& m_sessionsService;
    UsersService& m_usersService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;

    Session CreateSession(const std::string& userId);
};
