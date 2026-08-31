#include "deserializer/SocketEventDeserializer.h"
#include "log/Logger.h"
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
SocketServer::SocketServer(unsigned int Port, Logger& logger)
	: m_port(Port)
	, m_logger(logger)
{}

void SocketServer::BindConnectionUser(int clientSocket, const User& user)
{
	std::unordered_map<int, Connection>::iterator connectionsMapIterator = m_connectionsMap.find(clientSocket);
	if (connectionsMapIterator == m_connectionsMap.end())
	{
		m_logger.Error("Socket connection not found");
		return;
	}

	connectionsMapIterator->second.user = user;
	// NOTE: If key does not exist, this line creates it inside the map with an empty vector
	m_userClientSocketsMap[user.id].push_back(clientSocket);
}

void SocketServer::Close()
{
	close(m_socket);
}

const User* SocketServer::GetConnectionUser(int clientSocket)
{
	std::unordered_map<int, Connection>::iterator connectionsMapIterator = m_connectionsMap.find(clientSocket);
	if (connectionsMapIterator == m_connectionsMap.end())
	{
		m_logger.Error("Socket connection user not found");
		return nullptr;
	}

	if (!connectionsMapIterator->second.user.has_value())
	{
		m_logger.Error("Socket connection user not authenticated");
		return nullptr;
	}

	return &connectionsMapIterator->second.user.value();
}

void SocketServer::Init()
{
	// Creates socket file descriptor
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
	{
		m_logger.Error("Failed to create socket");
		exit(EXIT_FAILURE);
	}

	// Sets server address parameters
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = INADDR_ANY;
	m_address.sin_port = htons(m_port);

	// Binds the socket to the specified port
	int bind_result = bind(m_socket, reinterpret_cast<sockaddr *>(&m_address), sizeof(m_address));
	if (bind_result < 0)
	{
		m_logger.Error("Failed to bind socket");
		Close();
		exit(EXIT_FAILURE);
	}

	// Sets the server socket to non-blocking mode
	SetNonBlockingSocket(m_socket);
}

void SocketServer::Listen()
{
	// Listens for incoming connections
	int listenResult = listen(m_socket, MAX_PENDING_CONNECTIONS);
	if (listenResult < 0)
	{
		m_logger.Error("Failed to listen");
		Close();
		exit(EXIT_FAILURE);
	}

	// Creates epoll instance
    int epoll = epoll_create1(0);
	if (epoll == -1) {
		m_logger.Error("Failed to create epoll");
		exit(EXIT_FAILURE);
	}

    // Adds listening socket to epoll
	struct epoll_event event;
	event.events = EPOLLIN; // We are interested in read events
	event.data.fd = m_socket;

    int EpollCtlResult = epoll_ctl(epoll, EPOLL_CTL_ADD, m_socket, &event);
	if (EpollCtlResult == -1)
	{
		m_logger.Error("Failed to add listening socket to epoll");
		exit(EXIT_FAILURE);
	}

	m_logger.Info("Server listening on port " + std::to_string(m_port));

	struct epoll_event events[MAX_EPOLL_EVENTS];
	socklen_t address_length = sizeof(m_address);
	while (true)
	{
		int eventsCount = epoll_wait(epoll, events, MAX_EPOLL_EVENTS, -1);
		if (eventsCount == -1)
		{
			m_logger.Error("Failed to wait for client socket connections");
			break;
		}

		// Loops through all the events that have occurred
		for (int i = 0; i < eventsCount; i++) {
			// Existing connection
			if (events[i].data.fd != m_socket)
			{
				int clientSocket = events[i].data.fd;
				ReadClientSocket(clientSocket);

				continue;
			}

			// New connection
			// Accepts new connections
			int clientSocket = accept(m_socket, reinterpret_cast<sockaddr *>(&m_address), &address_length);
			// Handles non-blocking accept errors
			if (clientSocket < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
			{
				continue;
			}

			if (clientSocket == -1)
			{
				m_logger.Error("Failed to accept client socket connection");
				continue;
			}

			m_logger.Info("Accepted client socket " + std::to_string(clientSocket) + " new connection");

			Connection connection = {};
			connection.clientSocket = clientSocket;
			m_connectionsMap.insert(std::pair(clientSocket, connection));
			SetNonBlockingSocket(clientSocket);

			// Adds the new client socket to the epoll interest list
			event.events = EPOLLIN | EPOLLET; // Monitor for read events, edge-triggered
			event.data.fd = clientSocket;
			if (epoll_ctl(epoll, EPOLL_CTL_ADD, clientSocket, &event) == -1)
			{
				m_logger.Error("Failed to add client socket to epoll");
				CloseClientSocket(clientSocket);
			}
		}
	}
}

void SocketServer::On(SocketEventName socketEventName, const EventHandler& handler)
{
    On(socketEventName, {}, handler);
}

void SocketServer::On(SocketEventName socketEventName, const std::vector<EventMiddleware>& middlewares, const EventHandler& handler)
{
	if (!handler)
	{
		m_logger.Error("Cannot register a route without a handler");
		return;
	}

	std::unordered_map<SocketEventName, EventRoute>::iterator routesMapIterator = m_routesMap.find(socketEventName);
	if (routesMapIterator != m_routesMap.end())
	{
		m_logger.Warning("A route is already registered for this socket event. It will be replaced");
	}

	EventRoute route = {};
	// NOTE: Global middlewares are put first so they run before the ones registered on the route
	route.middlewares = m_middlewares;
	route.middlewares.insert(route.middlewares.end(), middlewares.begin(), middlewares.end());
	route.handler = handler;

	m_routesMap.insert(std::pair(socketEventName, route));
}

void SocketServer::OnError(const EventErrorHandler& errorHandler)
{
	m_errorHandler = errorHandler;
}

void SocketServer::SendAll(const std::string& serializedSocketEvent)
{
	for (const std::pair<int, Connection>& pair : m_connectionsMap)
	{
		SendTo(pair.first, serializedSocketEvent);
	}
}

void SocketServer::SendAllExcept(int exceptionClientSocket, const std::string& serializedSocketEvent)
{
	for (const std::pair<int, Connection>& pair : m_connectionsMap)
    {
		// If connection has no user, it means the client is not authenticated yet
		if (!pair.second.user.has_value()) continue;
        // If client socket is an exception (client socket not to send to)
        if (pair.first == exceptionClientSocket) continue;

        SendTo(pair.first, serializedSocketEvent);
    }
}

void SocketServer::SendAllExcept(std::unordered_map<int, bool> exceptionClientSocketsMap, const std::string& serializedSocketEvent)
{
	for (const std::pair<int, Connection>& pair : m_connectionsMap)
    {
		// If connection has no user, it means the client is not authenticated yet
		if (!pair.second.user.has_value()) continue;
        // If client socket is in the exception map (client socket not to send to)
        std::unordered_map<int, bool>::iterator exceptionClientSocketMapIterator = exceptionClientSocketsMap.find(pair.first);
        if (exceptionClientSocketMapIterator == exceptionClientSocketsMap.end()) continue;

        SendTo(pair.first, serializedSocketEvent);
    }
}

void SocketServer::SendTo(int clientSocket, const std::string& serializedSocketEvent)
{
	send(clientSocket, serializedSocketEvent.c_str(), serializedSocketEvent.length(), 0);
}

void SocketServer::SendTo(const std::string& userId, const std::string& serializedSocketEvent)
{
	std::unordered_map<std::string, std::vector<int>>::iterator userClientSocketsMapIterator = m_userClientSocketsMap.find(userId);
	if (userClientSocketsMapIterator == m_userClientSocketsMap.end())
	{
		m_logger.Error("User not found in user sockets map");
		return;
	}

	SendToMany(userClientSocketsMapIterator->second, serializedSocketEvent);
}

void SocketServer::SendToMany(std::vector<int> clientSockets, const std::string& serializedSocketEvent)
{
    for (int clientSocket : clientSockets)
    {
        SendTo(clientSocket, serializedSocketEvent);
    }
}

void SocketServer::Use(const EventMiddleware& middleware)
{
	if (!m_routesMap.empty())
	{
			m_logger.Error("Use() must be called before any route is registered. This middleware will not run");
			return;
	}

	m_middlewares.push_back(middleware);
}

// ***********
// * PRIVATE *
// ***********
 void SocketServer::CloseClientSocket(int clientSocket)
 {
	close(clientSocket);


	std::unordered_map<int, Connection>::iterator connectionsMapIterator = m_connectionsMap.find(clientSocket);
	if (connectionsMapIterator == m_connectionsMap.end())
	{
		m_logger.Warning("No socket connections found");
		return;
	}

	// If connection has a user
	const std::optional<User>& socketConnectionUser = connectionsMapIterator->second.user;
	if (socketConnectionUser.has_value())
	{
		// Removes socket from user's sockets
		std::unordered_map<std::string, std::vector<int>>::iterator userClientSocketsMapIterator = m_userClientSocketsMap.find(socketConnectionUser.value().id);
		userClientSocketsMapIterator->second.erase(std::remove(userClientSocketsMapIterator->second.begin(), userClientSocketsMapIterator->second.end(), clientSocket));

		// If after removing socket user does not have sockets anymore
		if (!userClientSocketsMapIterator->second.size())
		{
			// Removes sockets vector from map
			m_userClientSocketsMap.erase(socketConnectionUser.value().id);
		}
	}

	// Removes socket connection from map
	m_connectionsMap.erase(clientSocket);
 }

void SocketServer::ReadClientSocket(int clientSocket) {
    // Reads response from client socket
    std::array<char, BUFFER_SIZE> SerializedSocketEventBuffer = {0};
    ssize_t ReadResult = read(clientSocket, SerializedSocketEventBuffer.data(), BUFFER_SIZE);

	// Client socket response was read successfully
	if (ReadResult > 0)
    {

        const std::string& serializedSocketEvent(SerializedSocketEventBuffer.data());

		// NOTE: Declared before the try so the error handler can still use it if anything below throws
		EventContext context = {};
		context.clientSocket = clientSocket;

		try
		{
			SocketEventDeserializer socketEventDeserializer = {};
			SocketEvent<std::string> socketEvent = socketEventDeserializer.Deserialize(serializedSocketEvent);

			// NOTE: find, not operator[], which would insert an empty route for an unknown event name
			std::unordered_map<SocketEventName, EventRoute>::iterator routesMapIterator = m_routesMap.find(socketEvent.name);
			if (routesMapIterator == m_routesMap.end())
			{
				m_logger.Warning("No route registered for socket event from client socket " + std::to_string(clientSocket));
				return;
			}

			context.serializedPayload = socketEvent.payload;

			ExecuteRoute(routesMapIterator->second, context);

			m_logger.Info("Received socket event: " + socketEvent.name);
		}
		catch(const std::exception& exception)
		{
			if (m_errorHandler != nullptr)
			{
				m_errorHandler(exception, context);
				return;
			}

			m_logger.Error("Unhandled exception for client socket " + std::to_string(clientSocket) + ", " + exception.what());
		}
		
		
		return;
    }

	// Client socket closed connection
	if (ReadResult == 0)
	{
		m_logger.Info("Client socket " + std::to_string(clientSocket) + " disconnected");
        // NOTE:  This automatically removes it from epoll
		CloseClientSocket(clientSocket);

		return;
    }
	
	// Client socket response reading threw an error
	// NOTE: If errno is EAGAIN, that means we have read all data so we can continue to the next event.
	if (ReadResult == -1 && errno != EAGAIN)
	{
		m_logger.Error("Failed to read client socket");
		CloseClientSocket(clientSocket);
    }
}

void SocketServer::SetNonBlockingSocket(int socket)
{
	int Flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, Flags | O_NONBLOCK);
}

void SocketServer::ExecuteRoute(const EventRoute& route, EventContext& context)
{
	// NOTE: A middleware stops the route by throwing, which exits this loop before the handler runs
	for (const EventMiddleware& middleware : route.middlewares)
	{
		middleware(context);
	}

	route.handler(context);
}
