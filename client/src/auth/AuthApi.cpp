#include "auth/AuthApi.h"

// **********
// * PUBLIC *
// **********
AuthApi::AuthApi(std::shared_ptr<SocketClient> SocketClient) : m_SocketClient(SocketClient)
{}

void AuthApi::Login(const LoginParams& LoginParams)
{
    // Serializes login socket event
    LoginSocketEventPayload LoginSocketEventPayload(LoginParams.Email, LoginParams.Password);
    LoginSocketEvent LoginSocketEvent(LoginSocketEventPayload);
    std::string SerializedLoginSocketEvent = m_LoginSocketEventSerializer.Serialize(LoginSocketEvent);

    // Sends login socket event
    m_SocketClient->Send(SerializedLoginSocketEvent);
}

void AuthApi::Register(const RegisterParams& RegisterParams)
{
    // Serializes register socket event
    RegisterSocketEventPayload RegisterSocketEventPayload(RegisterParams.FirstName, RegisterParams.LastName, RegisterParams.Email, RegisterParams.Password);
    RegisterSocketEvent RegisterSocketEvent(RegisterSocketEventPayload);
    std::string SerializedRegisterSocketEvent = m_RegisterSocketEventSerializer.Serialize(RegisterSocketEvent);

    // Sends register socket event
    m_SocketClient->Send(SerializedRegisterSocketEvent);
}
