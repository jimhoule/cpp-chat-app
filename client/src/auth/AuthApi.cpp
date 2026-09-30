#include "auth/AuthApi.h"

#include "log/Logger.h"
#include "sessions/SessionStore.h"
#include "socket/SocketClient.h"

// **********
// * PUBLIC *
// **********
AuthApi::AuthApi(SocketClient& socketClient, SessionStore& sessionStore, Logger& logger)
    : m_socketClient(socketClient)
    , m_sessionStore(sessionStore)
    , m_logger(logger)
{
    SocketClient::EventHandler HandleAuthenticated = [this](const std::string& serializedAuthenticatedSocketEventPayload) {
        // Gets authenticated socket event payload
        const AuthenticatedSocketEventPayload authenticatedSocketEventPayload = m_authenticatedSocketEventPayloadDeserializer.Deserialize(serializedAuthenticatedSocketEventPayload);
        m_logger.Info("Authenticated user email: " + authenticatedSocketEventPayload.user.email);

        // Sends authenticated event to subsciptions
        const AuthenticatedEvent authenticatedEvent(authenticatedSocketEventPayload.user);
        m_authenticatedSubject.Notify(authenticatedEvent);
    };

    SocketClient::EventHandler HandleLoggedIn = [this](const std::string& serializedLoggedinSocketEventPayload) {
        // Gets logged in socket event payload
        const LoggedinSocketEventPayload loggedinSocketEventPayload = m_loggedinSocketEventPayloadDeserializer.Deserialize(serializedLoggedinSocketEventPayload);
        m_logger.Info("Logged in session id: " + loggedinSocketEventPayload.sessionId);

        // Saves session id in dedicated store
        m_sessionStore.Save(loggedinSocketEventPayload.sessionId);

        // Sends logged in event to subsciptions
        const LoggedInEvent loggedInEvent(loggedinSocketEventPayload.sessionId);
        m_loggedInSubject.Notify(loggedInEvent);
    };

    SocketClient::EventHandler HandleLoggedOut = [this](const std::string&) {
        // Removes session id from dedicated store
        m_sessionStore.Clear();

        // Sends logged out event to subsciptions
        const LoggedOutEvent loggedOutEvent;
        m_loggedOutSubject.Notify(loggedOutEvent);
    };

    SocketClient::EventHandler HandleRegistered = [this](const std::string& serializedRegisteredSocketEventPayload) {
        // Gets registered socket event payload
        const RegisteredSocketEventPayload registeredSocketEventPayload = m_registeredSocketEventPayloadDeserializer.Deserialize(serializedRegisteredSocketEventPayload);
        m_logger.Info("Registered session id: " + registeredSocketEventPayload.sessionId);

        // Saves session id in dedicated store
        m_sessionStore.Save(registeredSocketEventPayload.sessionId);

        // Sends registered event to subscriptions
        const RegisteredEvent registeredEvent(registeredSocketEventPayload.sessionId);
        m_registeredSubject.Notify(registeredEvent);
    };

    SocketClient::EventHandler HandleUserOnline = [this](const std::string& serializedUserOnlineSocketEventPayload) {
        // Gets user authenticated socket event payload
        const UserOnlineSocketEventPayload& userOnlineSocketEventPayload = m_userOnlineSocketEventPayloadDeserializer.Deserialize(serializedUserOnlineSocketEventPayload);
        m_logger.Info("Online user email: " + userOnlineSocketEventPayload.user.email);

        // Sends user online event to subcriptions
        const UserOnlineEvent userOnlineEvent(userOnlineSocketEventPayload.user);
        m_userOnlineSubject.Notify(userOnlineEvent);
    };

    m_socketClient.On(SocketEventName::AUTHENTICATED, HandleAuthenticated);
    m_socketClient.On(SocketEventName::LOGGEDIN, HandleLoggedIn);
    m_socketClient.On(SocketEventName::LOGGEDOUT, HandleLoggedOut);
    m_socketClient.On(SocketEventName::REGISTERED, HandleRegistered);
    m_socketClient.On(SocketEventName::USER_ONLINE, HandleUserOnline);
}

AuthApi::~AuthApi()
{
    m_socketClient.Off(SocketEventName::AUTHENTICATED);
    m_socketClient.Off(SocketEventName::LOGGEDIN);
    m_socketClient.Off(SocketEventName::LOGGEDOUT);
    m_socketClient.Off(SocketEventName::REGISTERED);
    m_socketClient.Off(SocketEventName::USER_ONLINE);
}

IObservable<AuthenticatedEvent>& AuthApi::GetAuthenticatedSubject()
{
    return m_authenticatedSubject;
}

IObservable<LoggedInEvent>& AuthApi::GetLoggedInSubject()
{
    return m_loggedInSubject;
}

IObservable<LoggedOutEvent>& AuthApi::GetLoggedOutSubject()
{
    return m_loggedOutSubject;
}

IObservable<RegisteredEvent>& AuthApi::GetRegisteredSubject()
{
    return m_registeredSubject;
}

IObservable<UserOnlineEvent>& AuthApi::GetUserOnlineSubject()
{
    return m_userOnlineSubject;
}

void AuthApi::Authenticate()
{
    // Gets stored session id
    const std::optional<std::string> sessionId = m_sessionStore.Load();
    if (!sessionId.has_value())
    {
        return;
    }

    // Serializes authenticate socket event
    const AuthenticateSocketEventPayload authenticateSocketEventPayload(sessionId.value());
    const AuthenticateSocketEvent authenticateSocketEvent(authenticateSocketEventPayload);
    const std::string serializedAuthenticateSocketEvent = m_authenticateSocketEventSerializer.Serialize(authenticateSocketEvent);

    // Sends authenticate socket event
    m_socketClient.Send(serializedAuthenticateSocketEvent);
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

void AuthApi::Logout()
{
    // Gets stored session id
    const std::optional<std::string> sessionId = m_sessionStore.Load();

    // Serializes logout socket event
    /**
     * NOTES:
     *  - If there is no session id, the connection still has to be unbound server side
     *  - An unknown session id is not an error, the server unbinds the connection and acknowledges anyway
     */
    const LogoutSocketEventPayload logoutSocketEventPayload(sessionId.value_or(""));
    const LogoutSocketEvent logoutSocketEvent(logoutSocketEventPayload);
    const std::string serializedLogoutSocketEvent = m_logoutSocketEventSerializer.Serialize(logoutSocketEvent);

    // Sends logout socket event
    m_socketClient.Send(serializedLogoutSocketEvent);
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

bool AuthApi::HasStoredSession() const
{
    const std::optional<std::string> sessionId = m_sessionStore.Load();

    return sessionId.has_value();
}
