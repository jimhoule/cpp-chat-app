#pragma once

#include "socket/SocketServerConnection.h"
#include "socket/SocketEventName.h"
#include "socket/SocketServerEventErrorHandler.h"
#include "socket/SocketServerEventHandler.h"
#include "socket/SocketServerEventMiddleware.h"
#include "socket/SocketServerEventRoute.h"

#include <arpa/inet.h>
#include <unordered_map>
#include <string>
#include <vector>

// Forward declarations
class Logger;

class SocketServer
{
public:
    SocketServer(unsigned int port, Logger& logger);

    void BindConnectionUser(int clientSocket, const User& user);
    void Close();
    const User* GetConnectionUser(int clientSocket);
    void Init();
    void Listen();
    void On(SocketEventName socketEventName, const SocketServerEventHandler& socketEventHandler);
    void On(SocketEventName socketEventName, const std::vector<SocketServerEventMiddleware>& middlewares, const SocketServerEventHandler& handler);
    void OnError(const SocketServerEventErrorHandler& errorHandler);
    void SendAll(const std::string& serializedSocketEvent);
    void SendAllExcept(int exceptionClientSocket, const std::string& serializedSocketEvent);
    void SendAllExcept(std::unordered_map<int, bool> exceptionClientSocketsMap, const std::string& serializedSocketEvent);
    void SendTo(int clientSocket, const std::string& serializedSocketEvent);
    void SendTo(const std::string& userId, const std::string& serializedSocketEvent);
    void SendToMany(std::vector<int> clientSockets, const std::string& serializedSocketEvent);
    void Use(const SocketServerEventMiddleware& middleware);

private:
    int m_Socket = 0;
    unsigned int m_Port = 0;
    sockaddr_in m_Address = {};
    std::unordered_map<int, SocketServerConnection> m_connectionsMap = {};
    std::unordered_map<std::string, std::vector<int>> m_userClientSocketsMap = {};
    //std::unordered_map<SocketEventName, SocketServerEventHandler> m_SocketEventHandlersMap = {};

    Logger& m_logger;
    // NOTE: The middlewares are globals
    std::vector<SocketServerEventMiddleware> m_middlewares = {};
    std::unordered_map<SocketEventName, SocketServerEventRoute> m_routesMap = {};
    SocketServerEventErrorHandler m_errorHandler = nullptr;

    void CloseClientSocket(int clientSocket);
    void ReadClientSocket(int clientSocket);
    void SetNonBlockingSocket(int socket);

    // NOTE: Runs every middleware in order, then the handler
    void ExecuteRoute(const SocketServerEventRoute& route, SocketEventContext& context);
};
