#include "deserializer/SocketEventDeserializer.h"
#include "socket/SocketServer.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

constexpr int MAX_PENDING_CONNECTIONS = 5;
constexpr int BUFFER_SIZE = 1000000;

// **********
// * PUBLIC *
// **********
SocketServer::SocketServer(unsigned int Port) : m_Port(Port)
{}

void SocketServer::Close()
{
	close(m_Socket);
}

void SocketServer::Init()
{
	// Creates socket file descriptor
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Sets server address parameters
	m_Address.sin_family = AF_INET;
	m_Address.sin_addr.s_addr = INADDR_ANY;
	m_Address.sin_port = htons(m_Port);

	// Binds the socket to the specified port
	int bind_result = bind(m_Socket, reinterpret_cast<sockaddr *>(&m_Address), sizeof(m_Address));
	if (bind_result < 0)
	{
		perror("bind failed");
		Close();
		exit(EXIT_FAILURE);
	}

	// Sets the server socket to non-blocking mode
	int fcntl_result = fcntl(m_Socket, F_SETFL, O_NONBLOCK);
	if (fcntl_result < 0)
	{
		perror("fcntl failed");
		Close();
		exit(EXIT_FAILURE);
	}
}

void SocketServer::Listen()
{
	// Listens for incoming connections
	int listen_result = listen(m_Socket, MAX_PENDING_CONNECTIONS);
	if (listen_result < 0)
	{
		perror("listen failed");
		Close();
		exit(EXIT_FAILURE);
	}

	std::cout << "Server listening on port " << m_Port << " ..." << "\n" << std::endl;

	socklen_t address_length = sizeof(m_Address);
	std::string message = "Hello from Server Socket!";
	while (true)
	{
		// Accepts new connections
		int ClientSocket = accept(m_Socket, reinterpret_cast<sockaddr *>(&m_Address), &address_length);
		// Handles non-blocking accept errors
		if (ClientSocket < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
			continue;
		}

		if (ClientSocket == -1)
		{
            std::cout << "No client socket" << std::endl;
			continue;
		}

		std::cout << "New connection was accepted for client socket " << ClientSocket << "\n" << std::endl;

		while (ClientSocket >= 0)
		{
			// Reads response from client
			std::array<char, BUFFER_SIZE> SerializedSocketEventBuffer = {0};
			ssize_t read_result = read(ClientSocket, SerializedSocketEventBuffer.data(), BUFFER_SIZE);
			if (read_result >= 0)
			{

				const std::string& SerializedSocketEvent(SerializedSocketEventBuffer.data());

                SocketEventDeserializer SocketEventDeserializer = {};
                SocketEvent<std::string> SocketEvent = SocketEventDeserializer.Deserialize(SerializedSocketEvent);

                SocketServerEventHandler HandleSocketEvent = m_SocketEventHandlersMap[SocketEvent.Name];
                HandleSocketEvent(SocketEvent.Payload, ClientSocket);

				std::cout << "Received socket event: " << SocketEvent.Name << std::endl;
			}
		}
	}
}

void SocketServer::On(SocketEventName SocketEventName, const SocketServerEventHandler& SocketEventHandler)
{
    m_SocketEventHandlersMap.insert(std::pair(SocketEventName, SocketEventHandler));
}

void SocketServer::Send(int ClientSocket, const std::string& SerializedSocketEvent)
{
	send(ClientSocket, SerializedSocketEvent.c_str(), SerializedSocketEvent.length(), 0);
}
