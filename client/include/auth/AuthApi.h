#pragma once

#include "auth/AuthenticatedEvent.h"
#include "auth/LoggedInEvent.h"
#include "auth/LoggedOutEvent.h"
#include "auth/RegisteredEvent.h"
#include "auth/UserOnlineEvent.h"
#include "deserializer/AuthenticatedSocketEventPayloadDeserializer.h"
#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"
#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"
#include "deserializer/UserOnlineSocketEventPayloadDeserializer.h"
#include "observer/IObservable.h"
#include "observer/Subject.h"
#include "serializer/AuthenticateSocketEventSerializer.h"
#include "serializer/LoginSocketEventSerializer.h"
#include "serializer/LogoutSocketEventSerializer.h"
#include "serializer/RegisterSocketEventSerializer.h"

// Forward declarations
class Logger;
class SessionStore;
class SocketClient;

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
    IObservable<LoggedInEvent>& GetLoggedInSubject();
    IObservable<LoggedOutEvent>& GetLoggedOutSubject();
    IObservable<RegisteredEvent>& GetRegisteredSubject();
    IObservable<UserOnlineEvent>& GetUserOnlineSubject();

    void Authenticate();
    void Login(const LoginParams& loginParams);
    void Logout();
    void Register(const RegisterParams& registerParams);
    bool HasStoredSession() const;

private:
    SocketClient& m_socketClient;

    Subject<AuthenticatedEvent> m_authenticatedSubject = {};
    Subject<LoggedInEvent> m_loggedInSubject = {};
    Subject<LoggedOutEvent> m_loggedOutSubject = {};
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

    SessionStore& m_sessionStore;

    Logger& m_logger;
};
