#include "deserializer/SocketEventDeserializer.h"
#include "socket/SocketServer.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

constexpr int MAX_EPOLL_EVENTS = 10;
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
		std::cerr << "Failed to create socket" << std::endl;
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
		std::cerr << "Failed to bind socket" << std::endl;
		Close();
		exit(EXIT_FAILURE);
	}

	// Sets the server socket to non-blocking mode
	SetNonBlockingSocket(m_Socket);
}

void SocketServer::Listen()
{
	// Listens for incoming connections
	int listen_result = listen(m_Socket, MAX_PENDING_CONNECTIONS);
	if (listen_result < 0)
	{
		std::cerr << "Failed to listen" << std::endl;
		Close();
		exit(EXIT_FAILURE);
	}

	// Creates epoll instance
    int Epoll = epoll_create1(0);
	if (Epoll == -1) {
		std::cerr << "Failed to create epoll" << std::endl;
		exit(EXIT_FAILURE);
	}

    // Adds listening socket to epoll
	struct epoll_event Event;
	Event.events = EPOLLIN; // We are interested in read events
	Event.data.fd = m_Socket;

    int EpollCtlResult = epoll_ctl(Epoll, EPOLL_CTL_ADD, m_Socket, &Event);
	if (EpollCtlResult == -1)
	{
		std::cerr << "Failed to add listening socket to epoll" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Server listening on port " << m_Port << " ..." << "\n" << std::endl;

	struct epoll_event Events[MAX_EPOLL_EVENTS];
	socklen_t address_length = sizeof(m_Address);
	while (true)
	{
		int EventsCount = epoll_wait(Epoll, Events, MAX_EPOLL_EVENTS, -1);
		if (EventsCount == -1)
		{
			std::cerr << "Failed to wait for client socket connections" << std::endl;
			break;
		}

		// Loops through all the events that have occurred
		for (int i = 0; i < EventsCount; i++) {
			// New connection
			if (Events[i].data.fd == m_Socket)
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
					std::cerr << "Failed to accept client socket connection" << std::endl;
					continue;
				}

				std::cout << "Accepted client socket " << ClientSocket << " new connection" << "\n" << std::endl;

				m_ClientSockets.push_back(ClientSocket);
				SetNonBlockingSocket(ClientSocket);

				// Adds the new client socket to the epoll interest list
				Event.events = EPOLLIN | EPOLLET; // Monitor for read events, edge-triggered
				Event.data.fd = ClientSocket;
				if (epoll_ctl(Epoll, EPOLL_CTL_ADD, ClientSocket, &Event) == -1)
				{
					std::cerr << "Failed to add client socket to epoll" << std::endl;
					CloseClientSocket(ClientSocket);
				}

				continue;
			}
			
			// Existing connection
			int ClientSocket = Events[i].data.fd;
			ReadClientSocket(ClientSocket);
		}
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
 void SocketServer::CloseClientSocket(int ClientSocket)
 {
	close(ClientSocket);
	m_ClientSockets.erase(std::remove(m_ClientSockets.begin(), m_ClientSockets.end(), ClientSocket), m_ClientSockets.end()); 
 }

void SocketServer::ReadClientSocket(int ClientSocket) {
    // Reads response from client socket
    std::array<char, BUFFER_SIZE> SerializedSocketEventBuffer = {0};
    ssize_t ReadResult = read(ClientSocket, SerializedSocketEventBuffer.data(), BUFFER_SIZE);

	// Client socket response was read successfully
	if (ReadResult > 0)
    {

        const std::string& SerializedSocketEvent(SerializedSocketEventBuffer.data());

        SocketEventDeserializer SocketEventDeserializer = {};
        SocketEvent<std::string> SocketEvent = SocketEventDeserializer.Deserialize(SerializedSocketEvent);

        SocketServerEventHandler HandleSocketEvent = m_SocketEventHandlersMap[SocketEvent.Name];
        HandleSocketEvent(SocketEvent.Payload, ClientSocket);

        std::cout << "Received socket event: " << SocketEvent.Name << std::endl;
		
		return;
    }

	// Client socket closed connection
	if (ReadResult == 0) {
		std::cout << "Client socket " << ClientSocket << " disconnected" << std::endl;
        // NOTE:  This automatically removes it from epoll
		CloseClientSocket(ClientSocket);

		return;
    }
	
	// Client socket response reading threw an error
	// NOTE: If errno is EAGAIN, that means we have read all data so we can continue to the next event.
	if (ReadResult == -1 && errno != EAGAIN) {
		std::cerr << "Failed to read client socket" << std::endl;
		CloseClientSocket(ClientSocket);
    }
}

void SocketServer::SetNonBlockingSocket(int Socket)
{
	int Flags = fcntl(Socket, F_GETFL, 0);
    fcntl(Socket, F_SETFL, Flags | O_NONBLOCK);
}
