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

        m_ClientSockets.push_back(ClientSocket);

        // Create a new thread to handle the client
        // The new_socket descriptor is passed by value to the thread function
        // std::function HandleClientSocketThread = [this, ClientSocket]() {
        //     ReadClientSocket(ClientSocket);
        // };
        m_ClientSocketThreads.emplace_back([this, ClientSocket]() {
            ReadClientSocket(ClientSocket);
        });

        // Detach the thread so it runs independently and we don't have to manage joins
        // This is a simple way to manage many clients without explicit thread management.
        // The OS will reclaim resources when the thread finishes.
        m_ClientSocketThreads.back().detach();

		// while (ClientSocket >= 0)
		// {
		// 	// Reads response from client
		// 	std::array<char, BUFFER_SIZE> SerializedSocketEventBuffer = {0};
		// 	ssize_t read_result = read(ClientSocket, SerializedSocketEventBuffer.data(), BUFFER_SIZE);
		// 	if (read_result >= 0)
		// 	{

		// 		const std::string& SerializedSocketEvent(SerializedSocketEventBuffer.data());

        //         SocketEventDeserializer SocketEventDeserializer = {};
        //         SocketEvent<std::string> SocketEvent = SocketEventDeserializer.Deserialize(SerializedSocketEvent);

        //         SocketServerEventHandler HandleSocketEvent = m_SocketEventHandlersMap[SocketEvent.Name];
        //         HandleSocketEvent(SocketEvent.Payload, ClientSocket);

		// 		std::cout << "Received socket event: " << SocketEvent.Name << std::endl;
		// 	}
		// }
	}
}

void SocketServer::On(SocketEventName SocketEventName, const SocketServerEventHandler& SocketEventHandler)
{
    m_SocketEventHandlersMap.insert(std::pair(SocketEventName, SocketEventHandler));
}

void SocketServer::SendAll(const std::string& SerializedSocketEvent)
{
    SendToMany(m_ClientSockets, SerializedSocketEvent);
}

void SocketServer::SendAllExcept(int ExceptionClientSocket, const std::string& SerializedSocketEvent)
{
    for (int ClientSocket : m_ClientSockets)
    {
        // If client socket is an exception (client socket not to send to)
        if (ClientSocket == ExceptionClientSocket) continue;

        SendTo(ClientSocket, SerializedSocketEvent);
    }
}

void SocketServer::SendAllExcept(std::unordered_map<int, bool> ExceptionClientSocketsMap, const std::string& SerializedSocketEvent)
{
    for (int ClientSocket : m_ClientSockets)
    {
        // If client socket is in the exception map (client socket not to send to)
        std::unordered_map<int, bool>::iterator ExceptionClientSocketIterator = ExceptionClientSocketsMap.find(ClientSocket);
        if (ExceptionClientSocketIterator == ExceptionClientSocketsMap.end()) continue;

        SendTo(ClientSocket, SerializedSocketEvent);
    }
}

void SocketServer::SendTo(int ClientSocket, const std::string& SerializedSocketEvent)
{
	send(ClientSocket, SerializedSocketEvent.c_str(), SerializedSocketEvent.length(), 0);
}

void SocketServer::SendToMany(std::vector<int> ClientSockets, const std::string& SerializedSocketEvent)
{
    for (int ClientSocket : ClientSockets)
    {
        SendTo(ClientSocket, SerializedSocketEvent);
    }
}

// ***********
// * PRIVATE *
// ***********
void SocketServer::ReadClientSocket(int ClientSocket) {
    // Reads response from client
    std::array<char, BUFFER_SIZE> SerializedSocketEventBuffer = {0};
    ssize_t ReadResult = read(ClientSocket, SerializedSocketEventBuffer.data(), BUFFER_SIZE);
    while (ReadResult >= 0)
    {

        const std::string& SerializedSocketEvent(SerializedSocketEventBuffer.data());

        SocketEventDeserializer SocketEventDeserializer = {};
        SocketEvent<std::string> SocketEvent = SocketEventDeserializer.Deserialize(SerializedSocketEvent);

        SocketServerEventHandler HandleSocketEvent = m_SocketEventHandlersMap[SocketEvent.Name];
        HandleSocketEvent(SocketEvent.Payload, ClientSocket);

        std::cout << "Received socket event: " << SocketEvent.Name << std::endl;
    }
}
