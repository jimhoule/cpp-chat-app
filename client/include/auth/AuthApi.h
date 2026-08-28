#pragma once

#include "auth/LoggedInEvent.h"
#include "auth/RegisteredEvent.h"
#include "auth/UserAuthenticatedEvent.h"
#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"
#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"
#include "deserializer/UserAuthenticatedSocketEventPayloadDeserializer.h"
#include "observer/IObservable.h"
#include "observer/Subject.h"
#include "serializer/LoginSocketEventSerializer.h"
#include "serializer/RegisterSocketEventSerializer.h"

// Forward declarations
class Logger;
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
    
    AuthApi(SocketClient& socketClient, Logger& logger);
    ~AuthApi();

    // NOTE: Exposes observable view only so callers cannot notify
    IObservable<LoggedInEvent>& GetLoggedInSubject();
    IObservable<RegisteredEvent>& GetRegisteredSubject();
    IObservable<UserAuthenticatedEvent>& GetUserAuthenticatedSubject();

    void Login(const LoginParams& loginParams);
    void Register(const RegisterParams& registerParams);

private:
    SocketClient& m_socketClient;

    Subject<LoggedInEvent> m_loggedInSubject = {};
    Subject<RegisteredEvent> m_registeredSubject = {};
    Subject<UserAuthenticatedEvent> m_userAuthenticatedSubject = {};

    LoginSocketEventSerializer m_loginSocketEventSerializer = {};
    LoggedinSocketEventPayloadDeserializer m_loggedinSocketEventPayloadDeserializer = {};

    RegisterSocketEventSerializer m_registerSocketEventSerializer = {};
    RegisteredSocketEventPayloadDeserializer m_registeredSocketEventPayloadDeserializer = {};

    UserAuthenticatedSocketEventPayloadDeserializer m_userAuthenticatedSocketEventPayloadDeserializer = {};

    Logger& m_logger;
};
