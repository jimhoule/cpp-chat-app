#pragma once

#include "socket/SocketConnection.h"
#include "socket/SocketEventName.h"
#include "socket/SocketServerEventHandler.h"

#include <arpa/inet.h>
#include <unordered_map>
#include <string>
#include <vector>

class SocketServer
{
public:
    SocketServer(unsigned int Port);

    void BindSocketConnectionUser(int clientSocket, const User& user);
    void Close();
    const User* GetSocketConnectionUser(int clientSocket);
    void Init();
    void Listen();
    void On(SocketEventName SocketEventName, const SocketServerEventHandler& SocketEventHandler);
    void SendAll(const std::string& SerializedSocketEvent);
    void SendAllExcept(int ExceptionClientSocket, const std::string& SerializedSocketEvent);
    void SendAllExcept(std::unordered_map<int, bool> ExceptionClientSocketsMap, const std::string& SerializedSocketEvent);
    void SendTo(int ClientSocket, const std::string& SerializedSocketEvent);
    void SendTo(const std::string& userId, const std::string& serializedSocketEvent);
    void SendToMany(std::vector<int> ClientSockets, const std::string& SerializedSocketEvent);

private:
    int m_Socket = 0;
    unsigned int m_Port = 0;
    sockaddr_in m_Address = {};
    //std::vector<int> m_ClientSockets = {};
    std::unordered_map<int, SocketConnection> m_socketConnectionsMap = {};
    std::unordered_map<std::string, std::vector<int>> m_userClientSocketsMap = {};
    std::unordered_map<SocketEventName, SocketServerEventHandler> m_SocketEventHandlersMap = {};

    void CloseClientSocket(int ClientSocket);
    void ReadClientSocket(int ClientSocket);
    void SetNonBlockingSocket(int Socket);
};
