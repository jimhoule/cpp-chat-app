#pragma once

#include "socket/SocketEvent.h"
#include "socket/SocketClientEventHandler.h"

#include <arpa/inet.h>
#include <map>
#include <string>

class SocketClient
{
public:
    SocketClient() = default;

    void Close();
    void Connect(int serverPort, const std::string &serverIpAddress);
    void Off(SocketEventName socketEventName);
    void On(SocketEventName socketEventName, const SocketClientEventHandler& socketEventHandler);
    void Read();
    void Send(const std::string &serializedSocketEvent);

private:
	int m_socket = 0;
    std::map<SocketEventName, SocketClientEventHandler> m_socketEventHandlersMap = {};
};
