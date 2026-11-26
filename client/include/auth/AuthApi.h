#pragma once

#include "auth/AuthenticatedEvent.h"
#include "auth/AuthenticateFailedEvent.h"
#include "auth/LoggedInEvent.h"
#include "auth/LoggedOutEvent.h"
#include "auth/RegisteredEvent.h"
#include "auth/UserOnlineEvent.h"
#include "deserializer/AuthenticatedSocketEventPayloadDeserializer.h"
#include "deserializer/ErrorSocketEventPayloadDeserializer.h"
#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"
#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"
#include "deserializer/UserOnlineSocketEventPayloadDeserializer.h"
#include "observer/IObservable.h"
#include "observer/Subject.h"
#include "serializer/AuthenticateSocketEventSerializer.h"
#include "serializer/LoginSocketEventSerializer.h"
#include "serializer/LogoutSocketEventSerializer.h"
#include "serializer/RegisterSocketEventSerializer.h"
#include "socket/SocketClient.h"
#include "views/UserView.h"

#include <optional>

// Forward declarations
class Logger;
class SessionStore;

class AuthApi
{
public:
    struct LoginParams
    {
        std::string email;
        std::string password;

    };

    struct RegisterParams
    {
        std::string firstName;
        std::string lastName;
        std::string email;
        std::string password;

    };
    
    AuthApi(SocketClient& socketClient, SessionStore& sessionStore, Logger& logger);
    ~AuthApi();

    // NOTE: Exposes observable view only so callers cannot notify
    IObservable<AuthenticatedEvent>& GetAuthenticatedSubject();
    IObservable<AuthenticateFailedEvent>& GetAuthenticateFailedSubject();
    IObservable<LoggedInEvent>& GetLoggedInSubject();
    IObservable<LoggedOutEvent>& GetLoggedOutSubject();
    IObservable<RegisteredEvent>& GetRegisteredSubject();
    IObservable<UserOnlineEvent>& GetUserOnlineSubject();

    void Authenticate();
    void Login(const LoginParams& loginParams);
    void Logout();
    void Register(const RegisterParams& registerParams);
    const UserView* GetCurrentUser() const;
    bool HasStoredSession() const;

private:
    SocketClient& m_socketClient;

    Subject<AuthenticatedEvent> m_authenticatedSubject = {};
    Subject<LoggedInEvent> m_loggedInSubject = {};
    Subject<LoggedOutEvent> m_loggedOutSubject = {};
    Subject<AuthenticateFailedEvent> m_authenticateFailedSubject = {};
    Subject<RegisteredEvent> m_registeredSubject = {};
    Subject<UserOnlineEvent> m_userOnlineSubject = {};

    AuthenticateSocketEventSerializer m_authenticateSocketEventSerializer = {};
    AuthenticatedSocketEventPayloadDeserializer m_authenticatedSocketEventPayloadDeserializer = {};

    LoginSocketEventSerializer m_loginSocketEventSerializer = {};
    LoggedinSocketEventPayloadDeserializer m_loggedinSocketEventPayloadDeserializer = {};

    LogoutSocketEventSerializer m_logoutSocketEventSerializer = {};

    RegisterSocketEventSerializer m_registerSocketEventSerializer = {};
    RegisteredSocketEventPayloadDeserializer m_registeredSocketEventPayloadDeserializer = {};

    UserOnlineSocketEventPayloadDeserializer m_userOnlineSocketEventPayloadDeserializer = {};

    ErrorSocketEventPayloadDeserializer m_errorSocketEventPayloadDeserializer = {};

    SessionStore& m_sessionStore;

    std::optional<UserView> m_currentUser = std::nullopt;

    Logger& m_logger;

    void HandleAuthenticated(const SocketClient::EventContext& context);
    void HandleError(const SocketClient::EventContext& context);
    void HandleLoggedIn(const SocketClient::EventContext& context);
    void HandleLoggedOut(const SocketClient::EventContext& context);
    void HandleRegistered(const SocketClient::EventContext& context);
    void HandleUserOnline(const SocketClient::EventContext& context);
};
