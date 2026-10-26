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
    SocketClient::EventHandler HandleAuthenticated = [this](const SocketClient::EventContext& context) {
        // Gets authenticated socket event payload
        const AuthenticatedSocketEventPayload authenticatedSocketEventPayload = m_authenticatedSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);
        m_logger.Info("Authenticated user email: " + authenticatedSocketEventPayload.user.email);

        // Sets current user
        m_currentUser = authenticatedSocketEventPayload.user;

        // Sends authenticated event to subsciptions
        const AuthenticatedEvent authenticatedEvent(authenticatedSocketEventPayload.user);
        m_authenticatedSubject.Notify(authenticatedEvent);
    };

    SocketClient::EventHandler HandleLoggedIn = [this](const SocketClient::EventContext& context) {
        // Gets logged in socket event payload
        const LoggedinSocketEventPayload loggedinSocketEventPayload = m_loggedinSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);
        m_logger.Info("Logged in session id: " + loggedinSocketEventPayload.sessionId);

        // Saves session id in dedicated store
        m_sessionStore.Save(loggedinSocketEventPayload.sessionId);

        // Sets current user
        m_currentUser = loggedinSocketEventPayload.user;

        // Sends logged in event to subsciptions
        const LoggedInEvent loggedInEvent(loggedinSocketEventPayload.sessionId);
        m_loggedInSubject.Notify(loggedInEvent);
    };

    SocketClient::EventHandler HandleLoggedOut = [this](const SocketClient::EventContext&) {
        // Removes session id from dedicated store
        m_sessionStore.Clear();

        // Removes current user
        m_currentUser.reset();

        // Sends logged out event to subsciptions
        const LoggedOutEvent loggedOutEvent;
        m_loggedOutSubject.Notify(loggedOutEvent);
    };

    SocketClient::EventHandler HandleRegistered = [this](const SocketClient::EventContext& context) {
        // Gets registered socket event payload
        const RegisteredSocketEventPayload registeredSocketEventPayload = m_registeredSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);
        m_logger.Info("Registered session id: " + registeredSocketEventPayload.sessionId);

        // Saves session id in dedicated store
        m_sessionStore.Save(registeredSocketEventPayload.sessionId);

        // Sets current user
        m_currentUser = registeredSocketEventPayload.user;

        // Sends registered event to subscriptions
        const RegisteredEvent registeredEvent(registeredSocketEventPayload.sessionId);
        m_registeredSubject.Notify(registeredEvent);
    };

    SocketClient::EventHandler HandleUserOnline = [this](const SocketClient::EventContext& context) {
        // Gets user authenticated socket event payload
        const UserOnlineSocketEventPayload& userOnlineSocketEventPayload = m_userOnlineSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);
        m_logger.Info("Online user email: " + userOnlineSocketEventPayload.user.email);

        // Sends user online event to subscriptions
        const UserOnlineEvent userOnlineEvent(userOnlineSocketEventPayload.user);
        m_userOnlineSubject.Notify(userOnlineEvent);
    };

    SocketClient::EventHandler HandleError = [this](const SocketClient::EventContext& context) {
        // Gets error socket event payload
        const ErrorSocketEventPayload& errorSocketEventPayload = m_errorSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        if (errorSocketEventPayload.failedEventName != SocketEventName::AUTHENTICATE)
        {
            return;
        }

        const std::string socketErrorCodeString = ConvertSocketErrorCodeToString(errorSocketEventPayload.code);
        m_logger.Warning("Authenticate failed: " + socketErrorCodeString);

        if (errorSocketEventPayload.code == SocketErrorCode::EXPIRED_SESSION || errorSocketEventPayload.code == SocketErrorCode::UNKNOWN_SESSION)
        {
            // Removes session id from dedicated store
            m_sessionStore.Clear();
        }

        // Sends authenticate failed event to subscriptions
        const AuthenticateFailedEvent authenticateFailedEvent;
        m_authenticateFailedSubject.Notify(authenticateFailedEvent);
    };

    m_socketClient.On(SocketEventName::AUTHENTICATED, this, HandleAuthenticated);
    m_socketClient.On(SocketEventName::LOGGEDIN, this, HandleLoggedIn);
    m_socketClient.On(SocketEventName::LOGGEDOUT, this, HandleLoggedOut);
    m_socketClient.On(SocketEventName::REGISTERED, this, HandleRegistered);
    m_socketClient.On(SocketEventName::USER_ONLINE, this, HandleUserOnline);
    m_socketClient.On(SocketEventName::ERROR, this, HandleError);
}

AuthApi::~AuthApi()
{
    m_socketClient.Off(SocketEventName::AUTHENTICATED, this);
    m_socketClient.Off(SocketEventName::LOGGEDIN, this);
    m_socketClient.Off(SocketEventName::LOGGEDOUT, this);
    m_socketClient.Off(SocketEventName::REGISTERED, this);
    m_socketClient.Off(SocketEventName::USER_ONLINE, this);
    m_socketClient.Off(SocketEventName::ERROR, this);
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

IObservable<AuthenticateFailedEvent>& AuthApi::GetAuthenticateFailedSubject()
{
    return m_authenticateFailedSubject;
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

const UserView* AuthApi::GetCurrentUser() const
{
    if (!m_currentUser.has_value())
    {
        return nullptr;
    }

    return &m_currentUser.value();
}

bool AuthApi::HasStoredSession() const
{
    const std::optional<std::string> sessionId = m_sessionStore.Load();

    return sessionId.has_value();
}
