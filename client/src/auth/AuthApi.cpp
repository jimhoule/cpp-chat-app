#include "auth/AuthApi.h"

#include "log/Logger.h"
#include "socket/SocketClient.h"

// **********
// * PUBLIC *
// **********
AuthApi::AuthApi(SocketClient& socketClient, Logger& logger)
    : m_socketClient(socketClient)
    , m_logger(logger)
{
    SocketClient::EventHandler HandleLoggedIn = [this](const std::string& serializedLoggedinSocketEventPayload) {
        // Gets logged in socket event payload
        const LoggedinSocketEventPayload loggedinSocketEventPayload = m_loggedinSocketEventPayloadDeserializer.Deserialize(serializedLoggedinSocketEventPayload);
        m_logger.Info("Logged in session id: " + loggedinSocketEventPayload.sessionId);

        // Sends logged in event to subsciptions
        const LoggedInEvent loggedInEvent(loggedinSocketEventPayload.sessionId);
        m_loggedInSubject.Notify(loggedInEvent);
    };

    SocketClient::EventHandler HandleRegistered = [this](const std::string& serializedRegisteredSocketEventPayload) {
        // Gets registered socket event payload
        const RegisteredSocketEventPayload registeredSocketEventPayload = m_registeredSocketEventPayloadDeserializer.Deserialize(serializedRegisteredSocketEventPayload);
        m_logger.Info("Registered session id: " + registeredSocketEventPayload.sessionId);

        // Sends registered event to subscriptions
        const RegisteredEvent registeredEvent(registeredSocketEventPayload.sessionId);
        m_registeredSubject.Notify(registeredEvent);
    };

    SocketClient::EventHandler HandleUserAuthenticated = [this](const std::string& serializedUserAuthenticatedSocketEventPayload) {
        // Gets user authenticated socket event payload
        const UserAuthenticatedSocketEventPayload& userAuthenticatedSocketEventPayload = m_userAuthenticatedSocketEventPayloadDeserializer.Deserialize(serializedUserAuthenticatedSocketEventPayload);
        m_logger.Info("Authenticated user ID: " + userAuthenticatedSocketEventPayload.user.id);

        // Sends user authenticated event to subcriptions
        const UserAuthenticatedEvent userAuthenticatedEvent(userAuthenticatedSocketEventPayload.user);
        m_userAuthenticatedSubject.Notify(userAuthenticatedEvent);
    };

    m_socketClient.On(SocketEventName::LOGGEDIN, HandleLoggedIn);
    m_socketClient.On(SocketEventName::REGISTERED, HandleRegistered);
    m_socketClient.On(SocketEventName::USER_AUTHENTICATED, HandleUserAuthenticated);
}

AuthApi::~AuthApi()
{
   m_socketClient.Off(SocketEventName::LOGGEDIN);
   m_socketClient.Off(SocketEventName::REGISTERED);
   m_socketClient.Off(SocketEventName::USER_AUTHENTICATED);
}

IObservable<LoggedInEvent>& AuthApi::GetLoggedInSubject()
{
    return m_loggedInSubject;
}

IObservable<RegisteredEvent>& AuthApi::GetRegisteredSubject()
{
    return m_registeredSubject;
}

IObservable<UserAuthenticatedEvent>& AuthApi::GetUserAuthenticatedSubject()
{
    return m_userAuthenticatedSubject;
}

void AuthApi::Login(const LoginParams& loginParams)
{
    // Serializes login socket event
    const LoginSocketEventPayload loginSocketEventPayload(loginParams.email, loginParams.password);
    const LoginSocketEvent loginSocketEvent(loginSocketEventPayload);
    const std::string serializedLoginSocketEvent = m_loginSocketEventSerializer.Serialize(loginSocketEvent);

    // Sends login socket event
    m_socketClient.Send(serializedLoginSocketEvent);
}

void AuthApi::Register(const RegisterParams& registerParams)
{
    // Serializes register socket event
    const RegisterSocketEventPayload registerSocketEventPayload(
        registerParams.firstName,
        registerParams.lastName,
        registerParams.email,
        registerParams.password
    );
    const RegisterSocketEvent registerSocketEvent(registerSocketEventPayload);
    const std::string serializedRegisterSocketEvent = m_registerSocketEventSerializer.Serialize(registerSocketEvent);

    // Sends register socket event
    m_socketClient.Send(serializedRegisterSocketEvent);
}
