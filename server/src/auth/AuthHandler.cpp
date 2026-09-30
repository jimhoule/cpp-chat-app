#include "auth/AuthHandler.h"

#include "auth/AuthService.h"
#include "log/Logger.h"
#include "exceptions/ExpectedException.h"
#include "socket/SocketErrorCode.h"

// **********
// * PUBLIC *
// **********
AuthHandler::AuthHandler(SocketServer& socketServer, AuthService& authService, Logger& logger)
    : m_socketServer(socketServer)
    , m_authService(authService)
    , m_logger(logger)
{}

SocketServer::EventHandler AuthHandler::GetAuthenticateHandler()
{
    return [this](const SocketServer::EventContext& context) {
        // Gets authenticate socket event payload
        const AuthenticateSocketEventPayload authenticateSocketEventPayload = m_authenticateSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Authenticates 
        AuthService::AuthenticateDto authenticateDto = {};
        authenticateDto.sessionId = authenticateSocketEventPayload.sessionId;
        const AuthService::AuthResult authResult = m_authService.Authenticate(authenticateDto);
        if (authResult.code != AuthService::AuthResultCode::OK)
        {
            const std::string authResultCodeString = m_authService.ConvertAuthResultCodeToString(authResult.code);
            const SocketErrorCode socketErrorCode = ConvertAuthResultCodeToSocketErrorCode(authResult.code);
            throw ExpectedException(socketErrorCode, "Authentication failed, " + authResultCodeString);
        }

        // Serializes authenticated socket event
        const AuthenticatedSocketEventPayload authenticatedSocketEventPayload(authResult.data.user);
        const AuthenticatedSocketEvent authenticatedSocketEvent(authenticatedSocketEventPayload);
        const std::string serializedAuthenticatedSocketEvent = m_authenticatedSocketEventSerializer.Serialize(authenticatedSocketEvent);

        // Sends authenticated socket event
        m_socketServer.SendTo(context.clientSocket, serializedAuthenticatedSocketEvent);

        // Sends user offline socket event to others
        SendUserOnlineSocketEvent(context.clientSocket, authResult.data.user);
    };
}

SocketServer::EventHandler AuthHandler::GetLoginHandler()
{
    return [this](const SocketServer::EventContext& context) {
        // Gets login socket event payload
        const LoginSocketEventPayload loginSocketEventPayload = m_loginSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Logs in 
        AuthService::LoginDto loginDto = {};
        loginDto.email = loginSocketEventPayload.email;
        loginDto.password = loginSocketEventPayload.password;
        const AuthService::AuthResult authResult = m_authService.Login(loginDto);
        if (authResult.code != AuthService::AuthResultCode::OK)
        {
            const std::string authResultCodeString = m_authService.ConvertAuthResultCodeToString(authResult.code);
            const SocketErrorCode socketErrorCode = ConvertAuthResultCodeToSocketErrorCode(authResult.code);
            throw ExpectedException(socketErrorCode, "Login failed for email " + loginDto.email + ", " + authResultCodeString);
        }

        // Serializes logged in socket event
        const LoggedinSocketEventPayload loggedinSocketEventPayload(authResult.data.sessionId, authResult.data.user);
        const LoggedinSocketEvent loggedinSocketEvent(loggedinSocketEventPayload);
        const std::string serializedLoggedinSocketEvent = m_loggedinSocketEventSerializer.Serialize(loggedinSocketEvent);

        // Sends logged in socket event
        m_socketServer.SendTo(context.clientSocket, serializedLoggedinSocketEvent);

        // Sends user offline socket event to others
        SendUserOnlineSocketEvent(context.clientSocket, authResult.data.user);
    };
}

SocketServer::EventHandler AuthHandler::GetLogoutHandler()
{
    return [this](const SocketServer::EventContext& context) {
        // Gets logout socket event payload
        const LogoutSocketEventPayload logoutSocketEventPayload = m_logoutSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Logs out
        AuthService::LogoutDto logoutDto = {};
        logoutDto.sessionId = logoutSocketEventPayload.sessionId;
        logoutDto.userId = context.user->id;
        AuthService::AuthResult authResult = m_authService.Logout(logoutDto);
        if (authResult.code != AuthService::AuthResultCode::OK)
        {
            const std::string authResultCodeString = m_authService.ConvertAuthResultCodeToString(authResult.code);
            const SocketErrorCode socketErrorCode = ConvertAuthResultCodeToSocketErrorCode(authResult.code);
            throw ExpectedException(socketErrorCode, "Logout failed, " + authResultCodeString);
        }

        // Serializes logged out socket event
        const LoggedoutSocketEventPayload loggedoutSocketEventPayload;
        const LoggedoutSocketEvent loggedoutSocketEvent(loggedoutSocketEventPayload);
        const std::string serializedLoggedoutSocketEvent = m_loggedoutSocketEventSerializer.Serialize(loggedoutSocketEvent);

        // Sends logged out socket event
        m_socketServer.SendTo(context.clientSocket, serializedLoggedoutSocketEvent);

        // Sends user offline socket event to others
        SendUserOfflineSocketEvent(context.clientSocket, context.user->id);
    };
}

SocketServer::EventHandler AuthHandler::GetRegisterHandler()
{
    return [this](const SocketServer::EventContext& context) {
        // Gets register socket event payload
        const RegisterSocketEventPayload registerSocketEventPayload = m_registerSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Generates access token
        AuthService::RegisterDto registerDto = {};
        registerDto.email = registerSocketEventPayload.email;
        registerDto.firstName = registerSocketEventPayload.firstName;
        registerDto.lastName = registerSocketEventPayload.lastName;
        registerDto.password = registerSocketEventPayload.password;
        const AuthService::AuthResult authResult = m_authService.Register(registerDto);
        if (authResult.code != AuthService::AuthResultCode::OK)
        {
            const std::string authResultCodeString = m_authService.ConvertAuthResultCodeToString(authResult.code);
            const SocketErrorCode socketErrorCode = ConvertAuthResultCodeToSocketErrorCode(authResult.code);
            throw ExpectedException(
                socketErrorCode,
                "Registration failed for email " + registerDto.email + ", " + authResultCodeString
            );
        }

        // Serializes registered socket event
        const RegisteredSocketEventPayload registeredSocketEventPayload(authResult.data.sessionId, authResult.data.user);
        const RegisteredSocketEvent registeredSocketEvent(registeredSocketEventPayload);
        const std::string serializedRegisteredSocketEvent = m_registeredSocketEventSerializer.Serialize(registeredSocketEvent);

        // Sends registered socket event
        m_socketServer.SendTo(context.clientSocket, serializedRegisteredSocketEvent);

        // Sends user offline socket event to others
        SendUserOnlineSocketEvent(context.clientSocket, authResult.data.user);
    };
}

// ***********
// * PRIVATE *
// ***********
void AuthHandler::SendUserOfflineSocketEvent(int clientSocket, const std::string& userId)
{
    // Unbinds user from server
    m_socketServer.UnbindConnectionUser(clientSocket);
    
    // Serializes user offline socket events
    const UserOfflineSocketEventPayload userOfflineSocketEventPayload(userId);
    const UserOfflineSocketEvent userOfflineSocketEvent(userOfflineSocketEventPayload);
    const std::string serializedUserOfflineSocketEvent = m_userOfflineSocketEventSerializer.Serialize(userOfflineSocketEvent);

    // Sends user offline socket event to others
    m_socketServer.SendAllExcept(clientSocket, serializedUserOfflineSocketEvent);
}

void AuthHandler::SendUserOnlineSocketEvent(int clientSocket, const User& user)
{
    // Binds user to server
    m_socketServer.BindConnectionUser(clientSocket, user);

    // Serializes user online socket event
    const UserOnlineSocketEventPayload userOnlineSocketEventPayload(user);
    const UserOnlineSocketEvent userOnlineSocketEvent(userOnlineSocketEventPayload);
    const std::string serializedUserOnlineSocketEvent = m_userOnlineSocketEventSerializer.Serialize(userOnlineSocketEvent);

    // Sends user authenticated socket event to others
    m_socketServer.SendAllExcept(clientSocket, serializedUserOnlineSocketEvent);
}

SocketErrorCode AuthHandler::ConvertAuthResultCodeToSocketErrorCode(AuthService::AuthResultCode authResultCode)
{
    switch (authResultCode)
    {
        case AuthService::AuthResultCode::UNKNOWN_USER:
        case AuthService::AuthResultCode::INVALID_PASSWORD:
            return SocketErrorCode::INVALID_CREDENTIALS;

        case AuthService::AuthResultCode::UNKNOWN_SESSION:
            return SocketErrorCode::UNKNOWN_SESSION;

        case AuthService::AuthResultCode::EXPIRED_SESSION:
            return SocketErrorCode::EXPIRED_SESSION;

        case AuthService::AuthResultCode::ORPHANED_SESSION:
            return SocketErrorCode::INTERNAL;

        case AuthService::AuthResultCode::EMAIL_ALREADY_USED:
            return SocketErrorCode::ALREADY_EXISTS;

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return SocketErrorCode::INTERNAL;
    }
}
