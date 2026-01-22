#include "auth/AuthHandler.h"

// **********
// * PUBLIC *
// **********
AuthHandler::AuthHandler(const SocketServer& SocketServer, const AuthService& AuthService) : m_SocketServer(SocketServer), m_AuthService(AuthService)
{}

SocketServerEventHandler AuthHandler::GetLoginHandlerFunction()
{
    return [this](const std::string& SerializedLoginSocketEventPayload, int ClientSocket) {
        // Gets login socket event payload
        const LoginSocketEventPayload& LoginSocketEventPayload = m_LoginSocketEventPayloadDeserializer.Deserialize(SerializedLoginSocketEventPayload);

        // Generates access token
        // TODO: Integrate jwt library
        const std::string& AccessToken = LoginSocketEventPayload.Email + "." + LoginSocketEventPayload.Password;

        // Serializes logged in socket event
        const LoggedinSocketEventPayload& LoggedinSocketEventPayload(AccessToken);
        const LoggedinSocketEvent& LoggedinSocketEvent(LoggedinSocketEventPayload);
        const std::string& SerializedLoggedinSocketEvent = m_LoggedinSocketEventSerializer.Serialize(LoggedinSocketEvent);

        // Sends logged in socket event
        m_SocketServer.Send(ClientSocket, SerializedLoggedinSocketEvent);
        // TODO: Send other event for user online
    };
}

SocketServerEventHandler AuthHandler::GetRegisterHandlerFunction()
{
    return [this](const std::string& SerializedRegisterSocketEventPayload, int ClientSocket) {
        // Gets register socket event payload
        const RegisterSocketEventPayload& RegisterSocketEventPayload = m_RegisterSocketEventPayloadDeserializer.Deserialize(SerializedRegisterSocketEventPayload);

        // Generates access token
        // TODO: Integrate jwt library
        const std::string& AccessToken = RegisterSocketEventPayload.FirstName + "." + RegisterSocketEventPayload.LastName + "." +RegisterSocketEventPayload.Email + "." + RegisterSocketEventPayload.Password;

        // Serializes registered socket event
        const RegisteredSocketEventPayload& RegisteredSocketEventPayload(AccessToken);
        const RegisteredSocketEvent& RegisteredSocketEvent(RegisteredSocketEventPayload);
        const std::string& SerializedRegisteredSocketEvent = m_RegisteredSocketEventSerializer.Serialize(RegisteredSocketEvent);

        // Sends registered socket event
        m_SocketServer.Send(ClientSocket, SerializedRegisteredSocketEvent);
        // TODO: Send other event for user online
    };
}
