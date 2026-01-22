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
	close(m_Socket);
}

void SocketClient::Connect(int ServerPort, const std::string &ServerIpAddress)
{
	// Creates socket file descriptor
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Sets the socket to non-blocking mode
	int FcntlResult = fcntl(m_Socket, F_SETFL, O_NONBLOCK);
	if (FcntlResult < 0)
	{
		perror("fcntl failed");
		exit(EXIT_FAILURE);
	}

	// Connects to server
	sockaddr_in ServerAddress = sockaddr_in{};
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(ServerPort);
	ServerAddress.sin_addr.s_addr = inet_addr(ServerIpAddress.c_str());

	int ConnectResult = connect(m_Socket, reinterpret_cast<sockaddr *>(&ServerAddress), sizeof(ServerAddress));
	if (ConnectResult < 0)
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

void SocketClient::On(SocketEventName SocketEventName, const SocketClientEventHandler& SocketEventHandler)
{
    m_SocketEventHandlersMap.insert(std::pair(SocketEventName, SocketEventHandler));
}

void SocketClient::Read()
{
    // Reads response from server
	std::array<char, BUFFER_SIZE> SerializedSocketEventBuffer = {0};
    ssize_t ReadResult = read(m_Socket, SerializedSocketEventBuffer.data(), BUFFER_SIZE);

    if (ReadResult == 0)
    {
        std::cout << "Server closed" << std::endl;
        return;
    }


    if (ReadResult > 0)
    {
        const std::string& SerializedSocketEvent(SerializedSocketEventBuffer.data());

        SocketEventDeserializer SocketEventDeserializer = {};
        SocketEvent<std::string> SocketEvent = SocketEventDeserializer.Deserialize(SerializedSocketEvent);

        SocketClientEventHandler HandleSocketEvent = m_SocketEventHandlersMap[SocketEvent.Name];
        HandleSocketEvent(SocketEvent.Payload);
    }
}

void SocketClient::Send(const std::string &SerializedSocketEvent)
{
	send(m_Socket, SerializedSocketEvent.c_str(), SerializedSocketEvent.length(), 0);
}
