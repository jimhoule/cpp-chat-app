#pragma once

#include "socket/SocketEvent.h"

#include <arpa/inet.h>
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

class SocketClient
{
public:
    using EventHandler = std::function<void(const std::string&)>;
    using EventHandlerOwner = const void*;

    SocketClient() = default;

    void Close();
    void Connect(int serverPort, const std::string &serverIpAddress);
    void Off(SocketEventName socketEventName, EventHandlerOwner owner);
    void On(SocketEventName socketEventName, EventHandlerOwner owner, const EventHandler& handler);
    void Read();
    void Send(const std::string &serializedSocketEvent);

private:
    struct EventHandlerEntry
    {
        EventHandlerOwner owner;
        EventHandler handler;
    };
    using EventHandlerEntries = std::vector<EventHandlerEntry>;
    using EventHandlersMap = std::unordered_map<SocketEventName, EventHandlerEntries>;

	int m_socket = 0;
    EventHandlersMap m_handlersMap = {};
};
