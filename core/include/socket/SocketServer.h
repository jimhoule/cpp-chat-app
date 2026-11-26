#pragma once

#include "deserializer/SocketEventDeserializer.h"
#include "socket/Framer.h"
#include "socket/SocketEventName.h"

#include <arpa/inet.h>
#include <exception>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Forward declarations
class Logger;

class SocketServer
{
public:
    struct EventContext
    {
        int clientSocket = -1;
        std::string serializedPayload = "";
        // NOTE: Set by the authentication middleware, stays null on routes that do not require it
        const std::string* userId = nullptr;
    };

    struct EventErrorContext
    {
        int clientSocket = -1;
        SocketEventName eventName = SocketEventName::ERROR;
    };

    struct UserBoundContext
    {
        std::string userId = "";
    };

    struct UserUnboundContext
    {
        std::string userId = "";
    };

    using EventErrorHandler = std::function<void(const std::exception&, EventErrorContext&)>;
    // NOTE: Pointer to a TOwner method that handles a socket event (ex: &UsersHandler::HandleSearchUsers)
    template<typename TOwner>
    using EventHandlerMethod = void (TOwner::*)(const EventContext&);
    using EventMiddleware = std::function<void(EventContext&)>;
    // NOTE: Pointer to a TOwner method called when a user's first socket is bound (ex: &UsersPresenceNotifier::HandleUserBound)
    template<typename TOwner>
    using UserBoundListenerMethod = void (TOwner::*)(const UserBoundContext&);
    // NOTE: Pointer to a TOwner method called when a user's last socket is unbound (ex: &UsersPresenceNotifier::HandleUserUnbound)
    template<typename TOwner>
    using UserUnboundListenerMethod = void (TOwner::*)(const UserUnboundContext&);

    SocketServer(unsigned int port, Logger& logger);

    void BindConnectionUser(int clientSocket, const std::string& userId);
    void Close();
    const std::string* GetConnectionUserId(int clientSocket);
    void Init();
    void Listen();
    void OnError(const EventErrorHandler& errorHandler);
    void SendAll(const std::string& serializedSocketEvent);
    void SendAllExcept(int exceptionClientSocket, const std::string& serializedSocketEvent);
    void SendAllExcept(const std::unordered_set<int>& exceptionClientSockets, const std::string& serializedSocketEvent);
    void SendTo(int clientSocket, const std::string& serializedSocketEvent);
    void SendTo(const std::string& userId, const std::string& serializedSocketEvent);
    void SendTo(const std::vector<int>& clientSockets, const std::string& serializedSocketEvent);
    void UnbindConnectionUser(int clientSocket);
    void Use(const EventMiddleware& middleware);

    // NOTE: Owner by reference because it is only the object the method runs on (never null since there is no remove)
    template<typename TOwner>
    void On(SocketEventName socketEventName, TOwner& owner, EventHandlerMethod<TOwner> method)
    {
        On(socketEventName, {}, owner, method);
    }

    // NOTE: Owner by reference because it is only the object the method runs on (never null since there is no remove)
    template<typename TOwner>
    void On(SocketEventName socketEventName, const std::vector<EventMiddleware>& middlewares, TOwner& owner, EventHandlerMethod<TOwner> method)
    {
        // NOTE: An empty handler lets AddRoute() reject a null method
        if (method == nullptr)
        {
            AddRoute(socketEventName, middlewares, nullptr);
            return;
        }

        /**
         * NOTES:
         *  - Wraps owner + method into an EventHandler so routing, middlewares and dispatch stay unchanged
         *  - Owner is only the object the method runs on (captured as a reference and never stored as a key)
         *  - Safe because modules (owners) live as long as the server
         */
        EventHandler handler = [&owner, method](const EventContext& context) {
            (owner.*method)(context);
        };

        AddRoute(socketEventName, middlewares, handler);
    }

    // NOTE: Owner by reference because it is only the object the method runs on (never null since there is no remove)
    template<typename TOwner>
    void OnUserBound(TOwner& owner, UserBoundListenerMethod<TOwner> method)
    {
        // NOTE: An empty listener lets AddUserBoundListener() reject a null method
        if (method == nullptr)
        {
            AddUserBoundListener(nullptr);
            return;
        }

        UserBoundListener listener = [&owner, method](const UserBoundContext& context) {
            (owner.*method)(context);
        };

        AddUserBoundListener(listener);
    }

    // NOTE: Owner by reference because it is only the object the method runs on (never null since there is no remove)
    template<typename TOwner>
    void OnUserUnbound(TOwner& owner, UserUnboundListenerMethod<TOwner> method)
    {
        // NOTE: An empty listener lets AddUserUnboundListener() reject a null method
        if (method == nullptr)
        {
            AddUserUnboundListener(nullptr);
            return;
        }

        UserUnboundListener listener = [&owner, method](const UserUnboundContext& context) {
            (owner.*method)(context);
        };

        AddUserUnboundListener(listener);
    }

private:
    struct Connection
    {
        int clientSocket = -1;
        std::optional<std::string> userId = std::nullopt;
        Framer framer = {};
        /**
         * NOTES:
         *  - Bytes waiting to be sent (send() only takes what fits in the socket, the rest waits here)
         *  - Sent once epoll says socket has room again (EPOLLOUT)
         */
        std::string writeBuffer = "";
    };

    using EventHandler = std::function<void(const EventContext&)>;

    struct EventRoute
    {
        std::vector<EventMiddleware> middlewares = {};
        EventHandler handler = nullptr;
    };

    using ConnectionsMap = std::unordered_map<int, Connection>;
    using EventRoutesMap = std::unordered_map<SocketEventName, EventRoute>;
    using UserBoundListener = std::function<void(const UserBoundContext&)>;
    using UserClientSocketsMap = std::unordered_map<std::string, std::vector<int>>;
    using UserUnboundListener = std::function<void(const UserUnboundContext&)>;

    int m_socket = -1;
    unsigned int m_port = 0;
    sockaddr_in m_address = {};
    ConnectionsMap m_connectionsMap = {};
    UserClientSocketsMap m_userClientSocketsMap = {};
    // NOTE: These middlewares are globals
    std::vector<EventMiddleware> m_middlewares = {};
    EventRoutesMap m_routesMap = {};
    EventErrorHandler m_errorHandler = nullptr;
    SocketEventDeserializer m_socketEventDeserializer = {};
    std::vector<UserBoundListener> m_userBoundListeners = {};
    std::vector<UserUnboundListener> m_userUnboundListeners = {};

    Logger& m_logger;

    void AddRoute(SocketEventName socketEventName, const std::vector<EventMiddleware>& middlewares, const EventHandler& handler);
    void AddUserBoundListener(const UserBoundListener& listener);
    void AddUserUnboundListener(const UserUnboundListener& listener);
    void CloseClientSocket(int clientSocket);
    std::optional<SocketEvent<std::string>> DeserializeSocketEvent(int clientSocket, const std::string& serializedSocketEvent);
    void DispatchSocketEvent(int clientSocket, const std::string& serializedSocketEvent);
    // NOTE: Runs every middleware in order, then the handler
    void ExecuteRoute(const EventRoute& route, EventContext& context);
    void ReadClientSocket(int clientSocket);
    void SendWriteBuffer(Connection& connection);
    void SetNonBlockingSocket(int socket);
    void WriteClientSocket(int clientSocket);
};
