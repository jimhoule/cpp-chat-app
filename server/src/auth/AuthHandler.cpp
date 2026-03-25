#include "auth/AuthHandler.h"

// **********
// * PUBLIC *
// **********
AuthHandler::AuthHandler(const SocketServer& SocketServer, const AuthService& AuthService) : m_SocketServer(SocketServer), m_AuthService(AuthService)
{}

SocketServerEventHandler AuthHandler::GetLoginHandler()
{
    return [this](const std::string& SerializedLoginSocketEventPayload, int ClientSocket) {
        // Gets login socket event payload
        const LoginSocketEventPayload& LoginSocketEventPayload = m_LoginSocketEventPayloadDeserializer.Deserialize(SerializedLoginSocketEventPayload);

        // Generates access token
        // TODO: Create auth service and integrate jwt library
        const std::string& AccessToken = LoginSocketEventPayload.Email + "." + LoginSocketEventPayload.Password;

        // Serializes logged in socket event
        const LoggedinSocketEventPayload& LoggedinSocketEventPayload(AccessToken);
        const LoggedinSocketEvent& LoggedinSocketEvent(LoggedinSocketEventPayload);
        const std::string& SerializedLoggedinSocketEvent = m_LoggedinSocketEventSerializer.Serialize(LoggedinSocketEvent);

        // Sends logged in socket event
        m_SocketServer.SendTo(ClientSocket, SerializedLoggedinSocketEvent);

        // Sends user authenticated socket event to others
        SendUserAuthenticatedSocketEvent(ClientSocket, AccessToken);
    };
}

SocketServerEventHandler AuthHandler::GetRegisterHandler()
{
    return [this](const std::string& SerializedRegisterSocketEventPayload, int ClientSocket) {
        // Gets register socket event payload
        const RegisterSocketEventPayload& RegisterSocketEventPayload = m_RegisterSocketEventPayloadDeserializer.Deserialize(SerializedRegisterSocketEventPayload);

        // Generates access token
        // TODO: Create auth service and integrate jwt library
        const std::string& AccessToken = RegisterSocketEventPayload.FirstName + "." + RegisterSocketEventPayload.LastName + "." +RegisterSocketEventPayload.Email + "." + RegisterSocketEventPayload.Password;

        // Serializes registered socket event
        const RegisteredSocketEventPayload& RegisteredSocketEventPayload(AccessToken);
        const RegisteredSocketEvent& RegisteredSocketEvent(RegisteredSocketEventPayload);
        const std::string& SerializedRegisteredSocketEvent = m_RegisteredSocketEventSerializer.Serialize(RegisteredSocketEvent);

        // Sends registered socket event
        m_SocketServer.SendTo(ClientSocket, SerializedRegisteredSocketEvent);

        // Sends user authenticated socket event to others
        SendUserAuthenticatedSocketEvent(ClientSocket, AccessToken);
    };
}

// ***********
// * PRIVATE *
// ***********
void AuthHandler::SendUserAuthenticatedSocketEvent(int ClientSocket, const std::string& AccessToken)
{
    // TODO: Decode access token to get user ID and fetch user
    User FakeUser = {};
    FakeUser.ID = "Fake ID";
    FakeUser.FirstName = "Fake Firstname";
    FakeUser.LastName = "Fake Lastname";
    FakeUser.ImageUrl = "https://www.fakeimageurl.com";

    // Serializes user authenticated socket event
    const UserAuthenticatedSocketEventPayload& UserAuthenticatedSocketEventPayload(FakeUser);
    const UserAuthenticatedSocketEvent& UserAuthenticatedSocketEvent(UserAuthenticatedSocketEventPayload);
    const std::string& SerializedUserAuthenticatedSocketEvent = m_UserAuthenticatedSocketEventSerializer.Serialize(UserAuthenticatedSocketEvent);

    // Sends user authenticated socket event to others
    m_SocketServer.SendAllExcept(ClientSocket, SerializedUserAuthenticatedSocketEvent);
}
