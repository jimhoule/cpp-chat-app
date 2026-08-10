#include "auth/AuthService.h"

std::string ConvertAuthResultCodeToString(AuthResultCode authResultCode)
{
    switch (authResultCode)
    {
        case AuthResultCode::OK:
            return "OK";

        case AuthResultCode::USER_NOT_FOUND:
            return "USER NOT FOUND";

        case AuthResultCode::INVALID_PASSWORD:
            return "INVALID PASSWORD";

        case AuthResultCode::EMAIL_ALREADY_USED:
            return "EMAIL ALREADY USED";

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return "Unknown auth result code";
    }
}

// **********
// * PUBLIC *
// **********
AuthService::AuthService(SessionsService& sessionsService, UsersService& usersService, Logger& logger)
    : m_sessionsService(sessionsService)
    , m_usersService(usersService)
    , m_logger(logger)
{}

AuthResult AuthService::Login(const LoginDto& loginDto)
{
    AuthResult authResult = {};
    UserResult userResult = {};

    // Checks if user with this email exists
    FindUserByEmailDto findUserByEmailDto = {};
    findUserByEmailDto.email = loginDto.email;
    userResult = m_usersService.FindByEmail(findUserByEmailDto);
    if (!userResult.data.has_value())
    {
        authResult.code = AuthResultCode::USER_NOT_FOUND;
        return authResult;
    }

    const User& user = userResult.data.value();

    // Validates password
    VerifyUserPasswordDto verifyUserPasswordDto = {};
    verifyUserPasswordDto.password = loginDto.password;
    verifyUserPasswordDto.hashedPassword = user.password;
    userResult = m_usersService.VerifyPassword(verifyUserPasswordDto);
    if (userResult.code != UsersResultCode::OK)
    {
        authResult.code = AuthResultCode::INVALID_PASSWORD;
        return authResult;
    }

    // Creates session
    const Session session = CreateSession(user.id);

    authResult.data.sessionId = session.id;
    authResult.data.user = user;

    return authResult;
}

AuthResult AuthService::Register(const RegisterDto& registerDto)
{
    AuthResult authResult = {};

    // Creates user
    CreateUserDto createUserDto = {};
    createUserDto.email = registerDto.email;
    createUserDto.firstName = registerDto.firstName;
    createUserDto.lastName = registerDto.lastName;
    createUserDto.password = registerDto.password;
    const UserResult userResult = m_usersService.Create(createUserDto);
    if (userResult.code != UsersResultCode::OK)
    {
        authResult.code = AuthResultCode::EMAIL_ALREADY_USED;
        return authResult;
    }

    const User& user = userResult.data.value();
    
    // Creates session
    const Session session = CreateSession(user.id);

    authResult.data.sessionId =  session.id;
    authResult.data.user = user;

    return authResult;
}

// ***********
// * PRIVATE *
// ***********
Session AuthService::CreateSession(const std::string& userId)
{
    CreateSessionDto createSessionDto = {};
    createSessionDto.userId = userId;
    const SessionResult sessionResult = m_sessionsService.Create(createSessionDto);

    return sessionResult.data.value();
}
