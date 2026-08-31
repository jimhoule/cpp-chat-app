#include "socket/SocketClient.h"
#include "deserializer/SocketEventDeserializer.h"

#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

constexpr int BUFFER_SIZE = 1024;

// **********
// * PUBLIC *
// **********
void SocketClient::Close()
{
	close(m_socket);
}

void SocketClient::Connect(int serverPort, const std::string &serverIpAddress)
{
	// Creates socket file descriptor
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Sets the socket to non-blocking mode
	int fcntlResult = fcntl(m_socket, F_SETFL, O_NONBLOCK);
	if (fcntlResult < 0)
	{
		perror("fcntl failed");
		exit(EXIT_FAILURE);
	}

	// Connects to server
	sockaddr_in serverAddress = sockaddr_in{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);
	serverAddress.sin_addr.s_addr = inet_addr(serverIpAddress.c_str());

	int connectResult = connect(m_socket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));
	if (connectResult < 0)
	{
		// Non-blocking connect will return immediately
		// Checks errno to distinguish between connection in progress and connection failed
		if (errno != EINPROGRESS)
		{
			perror("connection failed");
			exit(EXIT_FAILURE);
		}
	}
}

void SocketClient::Off(SocketEventName socketEventName)
{
	m_handlersMap.erase(socketEventName);
}

void SocketClient::On(SocketEventName socketEventName, const EventHandler& handler)
{
    m_handlersMap.insert(std::pair(socketEventName, handler));
}

void SocketClient::Read()
{
    // Reads response from server
	std::array<char, BUFFER_SIZE> serializedSocketEventBuffer = {0};
    ssize_t readResult = read(m_socket, serializedSocketEventBuffer.data(), BUFFER_SIZE);

    if (readResult == 0)
    {
        std::cout << "Server closed" << std::endl;
        return;
    }


    if (readResult > 0)
    {
        const std::string& serializedSocketEvent(serializedSocketEventBuffer.data());

        SocketEventDeserializer socketEventDeserializer = {};
        SocketEvent<std::string> socketEvent = socketEventDeserializer.Deserialize(serializedSocketEvent);

		// NOTE: find, not operator[], which would insert an empty handler for an unknown event name and then call it
        std::map<SocketEventName, EventHandler>::iterator handlersMapIterator = m_handlersMap.find(socketEvent.name);
        if (handlersMapIterator == m_handlersMap.end())
        {
            std::cout << "No handler registered for socket event " << socketEvent.name << std::endl;
            return;
        }

        EventHandler HandleSocketEvent = handlersMapIterator->second;
        HandleSocketEvent(socketEvent.payload);
    }
}

void SocketClient::Send(const std::string &serializedSocketEvent)
{
	send(m_socket, serializedSocketEvent.c_str(), serializedSocketEvent.length(), 0);
}
