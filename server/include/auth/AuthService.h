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
        UNKNOWN_USER,
        UNKNOWN_SESSION,
        EXPIRED_SESSION,
        ORPHANED_SESSION,
        INVALID_PASSWORD,
        EMAIL_ALREADY_USED
    };

    struct Auth
    {
        std::string sessionId = "";
        User user = {};
    };
    using AuthResult = ServiceResult<AuthResultCode, Auth>;

     struct AuthenticateDto
    {
        std::string sessionId;
    };

    struct LoginDto
    {
        std::string email;
        std::string password;
    };

    struct LogoutDto
    {
        std::string sessionId;
        std::string userId;
    };

    struct RegisterDto
    {
        std::string email;
        std::string firstName;
        std::string lastName;
        std::string password;
    };

    AuthService(SessionsService& sessionsService, UsersService& usersService, Logger& logger);

    AuthResult Authenticate(const AuthenticateDto& authenticateDto);
    AuthResult Login(const LoginDto& loginDto);
    AuthResult Logout(const LogoutDto& logoutDto);
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
