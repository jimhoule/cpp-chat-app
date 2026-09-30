#include "auth/AuthService.h"

#include "log/Logger.h"

// **********
// * PUBLIC *
// **********
AuthService::AuthService(SessionsService& sessionsService, UsersService& usersService, Logger& logger)
    : m_sessionsService(sessionsService)
    , m_usersService(usersService)
    , m_logger(logger)
{}

AuthService::AuthResult AuthService::Authenticate(const AuthenticateDto& authenticateDto)
{
    AuthResult authResult = {};

    // Fetches session
    SessionsService::FindSessionByIdDto findSessionByIdDto = {};
    findSessionByIdDto.id = authenticateDto.sessionId;
    const SessionsService::SessionResult findSessionByIdResult = m_sessionsService.FindById(findSessionByIdDto);
    if (findSessionByIdResult.code == SessionsService::SessionsResultCode::EXPIRED_SESSION)
    {
        authResult.code = AuthResultCode::EXPIRED_SESSION;
        return authResult;
    }

    if (!findSessionByIdResult.data.has_value())
    {
        authResult.code = AuthResultCode::UNKNOWN_SESSION;
        return authResult;
    }

    const Session& session = findSessionByIdResult.data.value();

    // Fetches user
    UsersService::FindUserByIdDto findUserByIdDto = {};
    findUserByIdDto.id = session.userId;
    const UsersService::UserResult userResult = m_usersService.FindById(findUserByIdDto);
    if (!userResult.data.has_value())
    {
        authResult.code = AuthResultCode::ORPHANED_SESSION;
        return authResult;
    }

    // Updates session expired at
    // NOTE: A failure must not fail an authentication that already succeeded
    SessionsService::RefreshSessionDto refreshSessionDto = {};
    refreshSessionDto.id = session.id;
    const SessionsService::SessionResult refreshSessionResult = m_sessionsService.Refresh(refreshSessionDto);
    if (refreshSessionResult.code != SessionsService::SessionsResultCode::OK)
    {
        m_logger.Warning("Could not refresh session " + session.id);
    }

    authResult.data.sessionId = session.id;
    authResult.data.user = userResult.data.value();

    return authResult;
}

AuthService::AuthResult AuthService::Login(const LoginDto& loginDto)
{
    AuthResult authResult = {};

    // Checks if user with this email exists
    UsersService::FindUserByEmailDto findUserByEmailDto = {};
    findUserByEmailDto.email = loginDto.email;
    const UsersService::UserResult findUserByEmailResult = m_usersService.FindByEmail(findUserByEmailDto);
    if (!findUserByEmailResult.data.has_value())
    {
        authResult.code = AuthResultCode::UNKNOWN_USER;
        return authResult;
    }

    const User& user = findUserByEmailResult.data.value();

    // Validates password
    UsersService::VerifyUserPasswordDto verifyUserPasswordDto = {};
    verifyUserPasswordDto.password = loginDto.password;
    verifyUserPasswordDto.hashedPassword = user.password;
    const UsersService::UserResult verifyPasswordResult = m_usersService.VerifyPassword(verifyUserPasswordDto);
    if (verifyPasswordResult.code != UsersService::UsersResultCode::OK)
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

AuthService::AuthResult AuthService::Logout(const LogoutDto& logoutDto)
{
    AuthResult authResult = {};

    // Fetches session
    SessionsService::FindSessionByIdDto findSessionByIdDto = {};
    findSessionByIdDto.id = logoutDto.sessionId;
    SessionsService::SessionResult findSessionByIdResult = m_sessionsService.FindById(findSessionByIdDto);
    // NOTE: Nothing to delete means the caller already has what it asked for
    if (!findSessionByIdResult.data.has_value())
    {
        return authResult;
    }

    const Session& session = findSessionByIdResult.data.value();

    // Checks if session belongs to authenticated user
    if (session.userId != logoutDto.userId)
    {
        authResult.code = AuthResultCode::UNKNOWN_SESSION;
        return authResult;
    }

    // Deletes session
    SessionsService::DeleteSessionDto deleteSessionDto = {};
    deleteSessionDto.id = session.id;
    m_sessionsService.Delete(deleteSessionDto);

    return authResult;
}

AuthService::AuthResult AuthService::Register(const RegisterDto& registerDto)
{
    AuthResult authResult = {};

    // Creates user
    UsersService::CreateUserDto createUserDto = {};
    createUserDto.email = registerDto.email;
    createUserDto.firstName = registerDto.firstName;
    createUserDto.lastName = registerDto.lastName;
    createUserDto.password = registerDto.password;
    const UsersService::UserResult userResult = m_usersService.Create(createUserDto);
    if (userResult.code != UsersService::UsersResultCode::OK)
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

std::string AuthService::ConvertAuthResultCodeToString(AuthResultCode authResultCode)
{
    switch (authResultCode)
    {
        case AuthResultCode::OK:
            return "OK";

        case AuthResultCode::UNKNOWN_USER:
            return "UNKNOWN USER";

        case AuthResultCode::UNKNOWN_SESSION:
            return "UNKNOWN SESSION";

        case AuthResultCode::EXPIRED_SESSION:
            return "EXPIRED SESSION";

         case AuthResultCode::ORPHANED_SESSION:
            return "ORPHANED SESSION";

        case AuthResultCode::INVALID_PASSWORD:
            return "INVALID PASSWORD";

        case AuthResultCode::EMAIL_ALREADY_USED:
            return "EMAIL ALREADY USED";

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return "Unknown auth result code";
    }
}

// ***********
// * PRIVATE *
// ***********
Session AuthService::CreateSession(const std::string& userId)
{
    SessionsService::CreateSessionDto createSessionDto = {};
    createSessionDto.userId = userId;
    const SessionsService::SessionResult sessionResult = m_sessionsService.Create(createSessionDto);

    return sessionResult.data.value();
}
