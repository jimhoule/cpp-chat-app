#pragma once

#include "deserializer/SocketEventDeserializer.h"
#include "socket/Framer.h"
#include "socket/SocketEvent.h"

#include <arpa/inet.h>
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

class SocketClient
{
public:
struct EventContext
    {
        std::string serializedPayload = "";
    };

    using EventHandler = std::function<void(const EventContext&)>;
    using EventHandlerOwner = const void*;

    SocketClient() = default;

    void Close();
    void Connect(int serverPort, const std::string &serverIpAddress);
    void Off(SocketEventName socketEventName, EventHandlerOwner owner);
    void On(SocketEventName socketEventName, EventHandlerOwner owner, const EventHandler& handler);
    void Send(const std::string &serializedSocketEvent);
    void Update();

private:
    struct Connection
    {
        int socket = -1;
        Framer framer = {};
        // NOTE: Bytes waiting to be sent (SendWriteBuffer() sends them on the next frame)
        std::string writeBuffer = "";
    };

    struct EventHandlerEntry
    {
        EventHandlerOwner owner;
        EventHandler handler;
    };
    
    using EventHandlerEntries = std::vector<EventHandlerEntry>;
    using EventHandlersMap = std::unordered_map<SocketEventName, EventHandlerEntries>;

    Connection m_connection = {};
    EventHandlersMap m_handlersMap = {};
    SocketEventDeserializer m_socketEventDeserializer = {};

    void DispatchSocketEvent(const std::string& serializedSocketEvent);
    void Read();
    void SendWriteBuffer();
};
