#include "auth/AuthHandler.h"

#include "auth/AuthService.h"
#include "log/Logger.h"
#include "socket/SocketServer.h"

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
    return [this](const std::string& serializedLoginSocketEventPayload, int clientSocket) {
        // Gets login socket event payload
        const LoginSocketEventPayload loginSocketEventPayload = m_loginSocketEventPayloadDeserializer.Deserialize(serializedLoginSocketEventPayload);

        // Logs in 
        LoginDto loginDto = {};
        loginDto.email = loginSocketEventPayload.email;
        loginDto.password = loginSocketEventPayload.password;
        const AuthServiceResult authServiceResult = m_authService.Login(loginDto);
        if (!authServiceResult.user.has_value())
        {
            // TODO: Implement better error propagation system
            SendErrorSocketEvent(clientSocket, "Login failed for client socket " + std::to_string(clientSocket));
            return;
        }

        // Serializes logged in socket event
        const LoggedinSocketEventPayload loggedinSocketEventPayload(authServiceResult.sessionId);
        const LoggedinSocketEvent loggedinSocketEvent(loggedinSocketEventPayload);
        const std::string serializedLoggedinSocketEvent = m_loggedinSocketEventSerializer.Serialize(loggedinSocketEvent);

        // Sends logged in socket event
        m_socketServer.SendTo(clientSocket, serializedLoggedinSocketEvent);

        // Sends user authenticated socket event to others
        SendUserAuthenticatedSocketEvent(clientSocket, authServiceResult.user.value());
    };
}

SocketServerEventHandler AuthHandler::GetRegisterHandler()
{
    return [this](const std::string& serializedRegisterSocketEventPayload, int clientSocket) {
        // Gets register socket event payload
        const RegisterSocketEventPayload registerSocketEventPayload = m_registerSocketEventPayloadDeserializer.Deserialize(serializedRegisterSocketEventPayload);

        // Generates access token
        // TODO: Create auth service and integrate jwt library
        RegisterDto registerDto = {};
        registerDto.email = registerSocketEventPayload.email;
        registerDto.firstName = registerSocketEventPayload.firstName;
        registerDto.lastName = registerSocketEventPayload.lastName;
        registerDto.password = registerSocketEventPayload.password;
        const AuthServiceResult authServiceResult = m_authService.Register(registerDto);
        if (!authServiceResult.user.has_value())
        {
            // TODO: Implement better error propagation system
            SendErrorSocketEvent(clientSocket, "Registration failed for socket " + std::to_string(clientSocket));
            return;
        }

        // Serializes registered socket event
        const RegisteredSocketEventPayload registeredSocketEventPayload(authServiceResult.sessionId);
        const RegisteredSocketEvent registeredSocketEvent(registeredSocketEventPayload);
        const std::string serializedRegisteredSocketEvent = m_registeredSocketEventSerializer.Serialize(registeredSocketEvent);

        // Sends registered socket event
        m_socketServer.SendTo(clientSocket, serializedRegisteredSocketEvent);

        // Sends user authenticated socket event to others
        SendUserAuthenticatedSocketEvent(clientSocket, authServiceResult.user.value());
    };
}

// ***********
// * PRIVATE *
// ***********
void AuthHandler::SendErrorSocketEvent(int clientSocket, const std::string& message)
{
    // TODO: Send ERROR socket event
    m_logger.Warning(message);
}

void AuthHandler::SendUserAuthenticatedSocketEvent(int clientSocket, const User& user)
{
    // Binds user to server
    m_socketServer.BindSocketConnectionUser(clientSocket, user);

    // Serializes user authenticated socket event
    const UserAuthenticatedSocketEventPayload userAuthenticatedSocketEventPayload(user);
    const UserAuthenticatedSocketEvent userAuthenticatedSocketEvent(userAuthenticatedSocketEventPayload);
    const std::string serializedUserAuthenticatedSocketEvent = m_userAuthenticatedSocketEventSerializer.Serialize(userAuthenticatedSocketEvent);

    // Sends user authenticated socket event to others
    m_socketServer.SendAllExcept(clientSocket, serializedUserAuthenticatedSocketEvent);
}
