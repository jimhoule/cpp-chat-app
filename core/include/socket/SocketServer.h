#pragma once

#include "deserializer/SocketEventDeserializer.h"
#include "socket/Framer.h"
#include "socket/SocketEventName.h"
#include "views/UserView.h"

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
        const UserView* user = nullptr;
    };

    struct EventErrorContext
    {
        int clientSocket = -1;
        SocketEventName eventName = SocketEventName::ERROR;
    };

    using EventErrorHandler = std::function<void(const std::exception&, EventErrorContext&)>;
    using EventHandler = std::function<void(const EventContext&)>;
    using EventMiddleware = std::function<void(EventContext&)>;

    SocketServer(unsigned int port, Logger& logger);

    void BindConnectionUser(int clientSocket, const UserView& user);
    void Close();
    const UserView* GetConnectionUser(int clientSocket);
    void Init();
    void Listen();
    void On(SocketEventName socketEventName, const EventHandler& handler);
    void On(SocketEventName socketEventName, const std::vector<EventMiddleware>& middlewares, const EventHandler& handler);
    void OnError(const EventErrorHandler& errorHandler);
    void SendAll(const std::string& serializedSocketEvent);
    void SendAllExcept(int exceptionClientSocket, const std::string& serializedSocketEvent);
    void SendAllExcept(const std::unordered_set<int>& exceptionClientSockets, const std::string& serializedSocketEvent);
    void SendTo(int clientSocket, const std::string& serializedSocketEvent);
    void SendTo(const std::string& userId, const std::string& serializedSocketEvent);
    void SendTo(const std::vector<int>& clientSockets, const std::string& serializedSocketEvent);
    void UnbindConnectionUser(int clientSocket);
    void Use(const EventMiddleware& middleware);

private:
    struct Connection
    {
        int clientSocket = -1;
        std::optional<UserView> user = std::nullopt;
        Framer framer = {};
        /**
         * NOTES:
         *  - Bytes waiting to be sent (send() only takes what fits in the socket, the rest waits here)
         *  - Sent once epoll says socket has room again (EPOLLOUT)
         */
        std::string writeBuffer = "";
    };

    struct EventRoute
    {
        std::vector<EventMiddleware> middlewares = {};
        EventHandler handler = nullptr;
    };

    using EventRoutesMap = std::unordered_map<SocketEventName, EventRoute>;

    int m_socket = -1;
    unsigned int m_port = 0;
    sockaddr_in m_address = {};
    std::unordered_map<int, Connection> m_connectionsMap = {};
    std::unordered_map<std::string, std::vector<int>> m_userClientSocketsMap = {};
    // NOTE: The middlewares are globals
    std::vector<EventMiddleware> m_middlewares = {};
    EventRoutesMap m_routesMap = {};
    EventErrorHandler m_errorHandler = nullptr;
    SocketEventDeserializer m_socketEventDeserializer = {};
    Logger& m_logger;

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
