#pragma once

#include "socket/SocketEvent.h"

#include <arpa/inet.h>
#include <functional>
#include <map>
#include <string>

class SocketClient
{
public:
    using EventHandler = std::function<void(const std::string&)>;

    SocketClient() = default;

    void Close();
    void Connect(int serverPort, const std::string &serverIpAddress);
    void Off(SocketEventName socketEventName);
    void On(SocketEventName socketEventName, const EventHandler& handler);
    void Read();
    void Send(const std::string &serializedSocketEvent);

private:
	int m_socket = 0;
    std::map<SocketEventName, EventHandler> m_handlersMap = {};
};
