#pragma once

#include "models/User.h"
#include "socket/SocketEventName.h"

#include <arpa/inet.h>
#include <exception>
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

// Forward declarations
class Logger;

class SocketServer
{
public:
    struct Connection
    {
        int clientSocket = -1;
        std::optional<User> user = std::nullopt;
        std::string readBuffer = "";
    };

    struct EventContext
    {
        int clientSocket = -1;
        std::string serializedPayload = "";
        // NOTE: Set by the authentication middleware, stays null on routes that do not require it
        const User* user = nullptr;
    };

    using EventErrorHandler = std::function<void(const std::exception&, EventContext&)>;
    using EventHandler = std::function<void(const EventContext&)>;
    using EventMiddleware = std::function<void(EventContext&)>;

    struct EventRoute
    {
        std::vector<EventMiddleware> middlewares = {};
        EventHandler handler = nullptr;
    };

    SocketServer(unsigned int port, Logger& logger);

    void BindConnectionUser(int clientSocket, const User& user);
    void Close();
    const User* GetConnectionUser(int clientSocket);
    void Init();
    void Listen();
    void On(SocketEventName socketEventName, const EventHandler& handler);
    void On(SocketEventName socketEventName, const std::vector<EventMiddleware>& middlewares, const EventHandler& handler);
    void OnError(const EventErrorHandler& errorHandler);
    void SendAll(const std::string& serializedSocketEvent);
    void SendAllExcept(int exceptionClientSocket, const std::string& serializedSocketEvent);
    void SendAllExcept(std::unordered_map<int, bool> exceptionClientSocketsMap, const std::string& serializedSocketEvent);
    void SendTo(int clientSocket, const std::string& serializedSocketEvent);
    void SendTo(const std::string& userId, const std::string& serializedSocketEvent);
    void SendToMany(std::vector<int> clientSockets, const std::string& serializedSocketEvent);
    void Use(const EventMiddleware& middleware);

private:
    int m_socket = 0;
    unsigned int m_port = 0;
    sockaddr_in m_address = {};
    std::unordered_map<int, Connection> m_connectionsMap = {};
    std::unordered_map<std::string, std::vector<int>> m_userClientSocketsMap = {};
    //std::unordered_map<SocketEventName, SocketServerEventHandler> m_SocketEventHandlersMap = {};

    Logger& m_logger;
    // NOTE: The middlewares are globals
    std::vector<EventMiddleware> m_middlewares = {};
    std::unordered_map<SocketEventName, EventRoute> m_routesMap = {};
    EventErrorHandler m_errorHandler = nullptr;

    void CloseClientSocket(int clientSocket);
    void ReadClientSocket(int clientSocket);
    void SetNonBlockingSocket(int socket);

    // NOTE: Runs every middleware in order, then the handler
    void ExecuteRoute(const EventRoute& route, EventContext& context);
};
