#pragma once

#include "deserializer/SocketEventDeserializer.h"
#include "socket/Framer.h"
#include "socket/SocketEvent.h"

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

// Forward declarations
class Logger;

class SocketClient
{
public:
struct EventContext
    {
        std::string serializedPayload = "";
    };

    using EventHandler = std::function<void(const EventContext&)>;
    // NOTE: The pointer to a TOwner method that handles a socket event (ex: &UsersApi::HandleUsersSearched)
    template<typename TOwner>
    using EventHandlerMethod = void (TOwner::*)(const EventContext&);
    using EventHandlerOwner = const void*;

    SocketClient(Logger& logger);

    void Close();
    void Connect(int serverPort, const std::string &serverIpAddress);
    void Off(SocketEventName socketEventName, EventHandlerOwner owner);
    
    template<typename TOwner>
    void On(SocketEventName socketEventName, TOwner* owner, EventHandlerMethod<TOwner> method)
    {
        // NOTE: An empty handler lets AddHandler() reject a null method
        if (method == nullptr)
        {
            AddHandler(socketEventName, owner, nullptr);
            return;
        }

        /**
         * NOTES:
         *  - Wraps owner + method into a handler
         *  - Owner is captured by pointer and is also the key Off() uses to remove the handler
         *  - Safe because each owner calls Off() in its destructor
         */
        EventHandler handler = [owner, method](const EventContext& context) {
            (owner->*method)(context);
        };

        AddHandler(socketEventName, owner, handler);
    }

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

    Logger& m_logger;

    void AddHandler(SocketEventName socketEventName, EventHandlerOwner owner, const EventHandler& handler);
    void DispatchSocketEvent(const std::string& serializedSocketEvent);
    void Read();
    void SendWriteBuffer();
};
