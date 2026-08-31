#include "auth/AuthHandler.h"

#include "auth/AuthService.h"
#include "log/Logger.h"
#include "exceptions/ExpectedException.h"
#include "socket/SocketErrorCode.h"
#include "socket/SocketServer.h"
#include "socket/SocketEventContext.h"

// **********
// * PUBLIC *
// **********
AuthHandler::AuthHandler(SocketServer& socketServer, AuthService& authService, Logger& logger)
    : m_socketServer(socketServer)
    , m_authService(authService)
    , m_logger(logger)
{}

SocketServerEventHandler AuthHandler::GetLoginHandler()
{
    return [this](const SocketEventContext& context) {
        // Gets login socket event payload
        const LoginSocketEventPayload loginSocketEventPayload = m_loginSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Logs in 
        AuthService::LoginDto loginDto = {};
        loginDto.email = loginSocketEventPayload.email;
        loginDto.password = loginSocketEventPayload.password;
        const AuthService::AuthResult authResult = m_authService.Login(loginDto);
        if (authResult.code != AuthService::AuthResultCode::OK)
        {
            throw ExpectedException(SocketErrorCode::INVALID_CREDENTIALS, "Invalid email or password");
        }

        // Serializes logged in socket event
        const LoggedinSocketEventPayload loggedinSocketEventPayload(authResult.data.sessionId);
        const LoggedinSocketEvent loggedinSocketEvent(loggedinSocketEventPayload);
        const std::string serializedLoggedinSocketEvent = m_loggedinSocketEventSerializer.Serialize(loggedinSocketEvent);

        // Sends logged in socket event
        m_socketServer.SendTo(context.clientSocket, serializedLoggedinSocketEvent);

        // Sends user authenticated socket event to others
        SendUserAuthenticatedSocketEvent(context.clientSocket, authResult.data.user);
    };
}

SocketServerEventHandler AuthHandler::GetRegisterHandler()
{
    return [this](const SocketEventContext& context) {
        // Gets register socket event payload
        const RegisterSocketEventPayload registerSocketEventPayload = m_registerSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Generates access token
        // TODO: Create auth service and integrate jwt library
        AuthService::RegisterDto registerDto = {};
        registerDto.email = registerSocketEventPayload.email;
        registerDto.firstName = registerSocketEventPayload.firstName;
        registerDto.lastName = registerSocketEventPayload.lastName;
        registerDto.password = registerSocketEventPayload.password;
        const AuthService::AuthResult authResult = m_authService.Register(registerDto);
        if (authResult.code != AuthService::AuthResultCode::OK)
        {
            const std::string authResultCodeString = m_authService.ConvertAuthResultCodeToString(authResult.code);
            throw ExpectedException(SocketErrorCode::ALREADY_EXISTS, "Registration failed for email " + registerDto.email + ", " + authResultCodeString);
        }

        // Serializes registered socket event
        const RegisteredSocketEventPayload registeredSocketEventPayload(authResult.data.sessionId);
        const RegisteredSocketEvent registeredSocketEvent(registeredSocketEventPayload);
        const std::string serializedRegisteredSocketEvent = m_registeredSocketEventSerializer.Serialize(registeredSocketEvent);

        // Sends registered socket event
        m_socketServer.SendTo(context.clientSocket, serializedRegisteredSocketEvent);

        // Sends user authenticated socket event to others
        SendUserAuthenticatedSocketEvent(context.clientSocket, authResult.data.user);
    };
}

// ***********
// * PRIVATE *
// ***********
void AuthHandler::SendUserAuthenticatedSocketEvent(int clientSocket, const User& user)
{
    // Binds user to server
    m_socketServer.BindConnectionUser(clientSocket, user);

    // Serializes user authenticated socket event
    const UserAuthenticatedSocketEventPayload userAuthenticatedSocketEventPayload(user);
    const UserAuthenticatedSocketEvent userAuthenticatedSocketEvent(userAuthenticatedSocketEventPayload);
    const std::string serializedUserAuthenticatedSocketEvent = m_userAuthenticatedSocketEventSerializer.Serialize(userAuthenticatedSocketEvent);

    // Sends user authenticated socket event to others
    m_socketServer.SendAllExcept(clientSocket, serializedUserAuthenticatedSocketEvent);
}
