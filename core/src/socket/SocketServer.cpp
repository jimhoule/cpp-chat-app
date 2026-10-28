#include "socket/SocketServer.h"

#include "log/Logger.h"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

constexpr int MAX_EPOLL_EVENTS = 10;
constexpr int MAX_PENDING_CONNECTIONS = 5;
constexpr int BUFFER_SIZE = 4096;

// **********
// * PUBLIC *
// **********
SocketServer::SocketServer(unsigned int Port, Logger& logger)
	: m_port(Port)
	, m_logger(logger)
{}

void SocketServer::BindConnectionUser(int clientSocket, const UserView& user)
{
	std::unordered_map<int, Connection>::iterator connectionsMapIterator = m_connectionsMap.find(clientSocket);
	if (connectionsMapIterator == m_connectionsMap.end())
	{
		m_logger.Error("Socket connection not found");
		return;
	}

	/**
	 * NOTES:
	 * 	- Detaches connection from userit was bound to, if any, so it is neverlisted under two users
	 * 	- For the case for re authentication
	 */
	UnbindConnectionUser(clientSocket);

	connectionsMapIterator->second.user = user;
	// NOTE: If key does not exist, this line creates it inside the map with an empty vector
	m_userClientSocketsMap[user.id].push_back(clientSocket);
}

void SocketServer::Close()
{
	close(m_socket);
}

const UserView* SocketServer::GetConnectionUser(int clientSocket)
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

    /**
	 * NOTES:
	 * 	- Adds listening socket to epoll
	 * 	- We are interested in read events
	 */
	struct epoll_event interestEvent;
	interestEvent.events = EPOLLIN;
	interestEvent.data.fd = m_socket;

    int listeningSocketEpollResult = epoll_ctl(epoll, EPOLL_CTL_ADD, m_socket, &interestEvent);
	if (listeningSocketEpollResult == -1)
	{
		m_logger.Error("Failed to add listening socket to epoll");
		exit(EXIT_FAILURE);
	}

	m_logger.Info("Server listening on port " + std::to_string(m_port));

	struct epoll_event events[MAX_EPOLL_EVENTS];
	socklen_t address_length = sizeof(m_address);
	while (true)
	{
		int reeadyEventsCount = epoll_wait(epoll, events, MAX_EPOLL_EVENTS, -1);
		if (reeadyEventsCount == -1)
		{
			m_logger.Error("Failed to wait for client socket connections");
			break;
		}

		// Loops through all the events that have occurred
		for (int index = 0; index < reeadyEventsCount; index++) {
			const epoll_event& readyEvent = events[index];

			// EXISTING CONNECTION
			if (readyEvent.data.fd != m_socket)
			{
				int clientSocket = readyEvent.data.fd;

				/**
				 * NOTES:
				 * 	- Socket has room again so send what is waiting
				 * 	- Done before reading because reading can close the socket
				 */
				if (readyEvent.events & EPOLLOUT)
				{
					WriteClientSocket(clientSocket);
				}

				/**
				 * NOTES:
				 * 	- EPOLLERR / EPOLLHUP also go to ReadClientSocket()
				 * 	- read() fails or returns 0 and it closes the socket
				 */
				if (readyEvent.events & (EPOLLIN | EPOLLERR | EPOLLHUP))
				{
					ReadClientSocket(clientSocket);
				}

				continue;
			}

			// NEW CONNECTION
			int clientSocket = accept(m_socket, reinterpret_cast<sockaddr *>(&m_address), &address_length);
			// Handles non-blocking accept errors
			if (clientSocket == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
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

			/**
			 * NOTES:
			 * 	- Adds the new client socket to the epoll interest list
			 * 	- Monitor for read events, edge-triggered
			 * 	- EPOLLOUT stays on and it only fires when a full socket has room again
			 */
			interestEvent.events = EPOLLIN | EPOLLOUT | EPOLLET;
			interestEvent.data.fd = clientSocket;
			int clientSocketEpollResult = epoll_ctl(epoll, EPOLL_CTL_ADD, clientSocket, &interestEvent);
			if (clientSocketEpollResult == -1)
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

	EventRoutesMap::iterator routesMapIterator = m_routesMap.find(socketEventName);
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
	// NOTE: The map stores pair<const int, Connection> so without the int key being a const the reference binds to a copy of each connection
	for (const std::pair<const int, Connection>& pair : m_connectionsMap)
	{
		SendTo(pair.first, serializedSocketEvent);
	}
}

void SocketServer::SendAllExcept(int exceptionClientSocket, const std::string& serializedSocketEvent)
{
	// NOTE: The map stores pair<const int, Connection> so without the int key being a const the reference binds to a copy of each connection
	for (const std::pair<const int, Connection>& pair : m_connectionsMap)
    {
		// If connection has no user, it means the client is not authenticated yet
		if (!pair.second.user.has_value())
		{
			continue;
		}

        // If client socket is an exception (client socket not to send to)
        if (pair.first == exceptionClientSocket)
		{
			continue;
		}

        SendTo(pair.first, serializedSocketEvent);
    }
}

void SocketServer::SendAllExcept(const std::unordered_set<int>& exceptionClientSockets, const std::string& serializedSocketEvent)
{
	// NOTE: The map stores pair<const int, Connection> so without the int key being a const the reference binds to a copy of each connection
	for (const std::pair<const int, Connection>& pair : m_connectionsMap)
    {
		// If connection has no user, it means the client is not authenticated yet
		if (!pair.second.user.has_value())
		{
			continue;
		}

        // If client socket is in the exception set (client socket not to send to)
        std::unordered_set<int>::const_iterator exceptionClientSocketsIterator = exceptionClientSockets.find(pair.first);
        if (exceptionClientSocketsIterator != exceptionClientSockets.end())
		{
			continue;
		}

        SendTo(pair.first, serializedSocketEvent);
    }
}

void SocketServer::SendTo(int clientSocket, const std::string& serializedSocketEvent)
{
	std::unordered_map<int, Connection>::iterator connectionsMapIterator = m_connectionsMap.find(clientSocket);
	if (connectionsMapIterator == m_connectionsMap.end())
	{
		m_logger.Error("Socket connection not found for client socket " + std::to_string(clientSocket));
		return;
	}

	/**
	 * NOTES:
	 * 	- Adds the frame behind the bytes already written so frames always go out in order
	 * 	- Usually the socket has room and everything is sent right away
	 */
	Connection& connection = connectionsMapIterator->second;
	connection.writeBuffer += Framer::Encode(serializedSocketEvent);
	SendWriteBuffer(connection);
}

void SocketServer::SendTo(const std::string& userId, const std::string& serializedSocketEvent)
{
	// NOTE: User has no socket (offline) so there is nothing to send
	std::unordered_map<std::string, std::vector<int>>::iterator userClientSocketsMapIterator = m_userClientSocketsMap.find(userId);
	if (userClientSocketsMapIterator == m_userClientSocketsMap.end())
	{
		return;
	}

	SendTo(userClientSocketsMapIterator->second, serializedSocketEvent);
}

void SocketServer::SendTo(const std::vector<int>& clientSockets, const std::string& serializedSocketEvent)
{
	for (int clientSocket : clientSockets)
	{
		SendTo(clientSocket, serializedSocketEvent);
	}
}

void SocketServer::UnbindConnectionUser(int clientSocket)
{
	// Finds connection
	std::unordered_map<int, Connection>::iterator connectionsMapIterator = m_connectionsMap.find(clientSocket);
	if (connectionsMapIterator == m_connectionsMap.end())
	{
		m_logger.Warning("No socket connections found");
		return;
	}

	Connection& connection = connectionsMapIterator->second;

	// NOTE: A connection with no user is not bound so there is nothing to unbind
	if (!connection.user.has_value())
	{
		return;
	}

	// Finds user's sockets
	std::unordered_map<std::string, std::vector<int>>::iterator userClientSocketsMapIterator = m_userClientSocketsMap.find(connection.user.value().id);
	if (userClientSocketsMapIterator != m_userClientSocketsMap.end())
	{
		// Removes socket from user's sockets
		std::vector<int>& userClientSockets = userClientSocketsMapIterator->second;
		userClientSockets.erase(std::remove(userClientSockets.begin(), userClientSockets.end(), clientSocket), userClientSockets.end());

		// If after removing socket user does not have sockets anymore
		if (userClientSockets.empty())
		{
			// Removes sockets vector from map
			m_userClientSocketsMap.erase(userClientSocketsMapIterator);
		}
	}

	// Removes user from connection
	connection.user = std::nullopt;
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

	// Removes user connection and connection from user's socket
	UnbindConnectionUser(clientSocket);

	// Removes socket connection from map
	m_connectionsMap.erase(clientSocket);
}

std::optional<SocketEvent<std::string>> SocketServer::DeserializeSocketEvent(int clientSocket, const std::string& serializedSocketEvent)
{
	try
	{
		return m_socketEventDeserializer.Deserialize(serializedSocketEvent);
	}
	catch(const std::exception& exception)
	{
		// NOTE: Without an event name the client cannot tell which request failed
		m_logger.Warning("Dropped unreadable socket event from client socket " + std::to_string(clientSocket) + ", " + exception.what());
		return std::nullopt;
	}	
}

void SocketServer::DispatchSocketEvent(int clientSocket, const std::string& serializedSocketEvent)
{
	std::optional<SocketEvent<std::string>> socketEvent = DeserializeSocketEvent(clientSocket, serializedSocketEvent);
	if (!socketEvent.has_value())
	{
		return;
	}

	try
	{
		// NOTE: find, not operator[], which would insert an empty route for an unknown event name
		EventRoutesMap::iterator routesMapIterator = m_routesMap.find(socketEvent.value().name);
		if (routesMapIterator == m_routesMap.end())
		{
			m_logger.Warning("No route registered for socket event from client socket " + std::to_string(clientSocket));
			return;
		}

		EventContext context = {};
		context.clientSocket = clientSocket;
		context.serializedPayload = socketEvent.value().payload;
		ExecuteRoute(routesMapIterator->second, context);

		m_logger.Info("Received socket event: " + socketEvent.value().name);
	}
	catch(const std::exception& exception)
	{
		if (m_errorHandler != nullptr)
		{
			EventErrorContext context = {};
			context.clientSocket = clientSocket;
			context.eventName = socketEvent.value().name;
			m_errorHandler(exception, context);
			return;
		}

		m_logger.Error("Unhandled exception for client socket " + std::to_string(clientSocket) + ", " + exception.what());
	}
}

void SocketServer::ExecuteRoute(const EventRoute& route, EventContext& context)
{
	// NOTE: A middleware stops the route by throwing which exits this loop before the handler runs
	for (const EventMiddleware& middleware : route.middlewares)
	{
		middleware(context);
	}

	route.handler(context);
}

void SocketServer::ReadClientSocket(int clientSocket)
{
	// Finds connection
	std::unordered_map<int, Connection>::iterator connectionsMapIterator = m_connectionsMap.find(clientSocket);
	if (connectionsMapIterator == m_connectionsMap.end())
	{
		m_logger.Warning("No socket connections found");
		return;
	}

	Framer& framer = connectionsMapIterator->second.framer;

	/**
	 * NOTES:
	 * 	- Reads until client socket is empty
	 * 	- Client sockets are edge triggered (EPOLLET) and epoll does not notify again for bytes left in the socket
	 */
	std::array<char, BUFFER_SIZE> chunk = {};
	while (true)
	{
		ssize_t readResult = read(clientSocket, chunk.data(), BUFFER_SIZE);

		// Adds chunk to frame
		if (readResult > 0)
		{
			framer.Append(chunk.data(), readResult);
			continue;
		}

		// NOTE: EAGAIN means nothing more to read for now
		if (readResult == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
			break;
		}

		// NOTE: Client socket closed connection
		if (readResult == 0)
		{
			m_logger.Info("Client socket " + std::to_string(clientSocket) + " disconnected");
		}
		// NOTE: read result is -1 and errno is other than EAGAIN or EWOULDBLOCK
		else
		{
			m_logger.Error("Failed to read client socket " + std::to_string(clientSocket) + ", " + std::strerror(errno));
		}

		// NOTE: This automatically removes it from epoll
		CloseClientSocket(clientSocket);

		return;
	}

	/**
	 * NOTES:
	 * 	- Can return several events (sent close together) or none (payload not fully arrived yet)
	 * 	- Throws when stream is out of sync (ex: client without framing) and nothing can recover it
	 * 	- The try only covers ExtractFrames() so an std::length_error thrown by a handler is not reported
	 * 	  as an invalid frame
	 */
	std::vector<std::string> serializedSocketEvents = {};
	try
	{
		serializedSocketEvents = framer.ExtractFrames();
	}
	catch(const std::length_error& exception)
	{
		m_logger.Warning("Invalid frame from client socket " + std::to_string(clientSocket) + ", " + exception.what());
		CloseClientSocket(clientSocket);

		return;
	}

	for (const std::string& serializedSocketEvent : serializedSocketEvents)
	{
		DispatchSocketEvent(clientSocket, serializedSocketEvent);
	}
}

void SocketServer::SendWriteBuffer(Connection& connection)
{
	// NOTE: How many bytes of write buffer have been sent so far
	std::size_t sentSize = 0;
	while (sentSize < connection.writeBuffer.size())
	{
		// NOTE: Using MSG_NOSIGNAL so sending to a client that is gone returns an error instead of killing the server (SIGPIPE)
		std::size_t size = connection.writeBuffer.size() - sentSize;
		ssize_t sendResult = send(connection.clientSocket, connection.writeBuffer.data() + sentSize, size, MSG_NOSIGNAL);

		// Sends the rest of the bytes that did not go out
		if (sendResult > 0)
		{
			sentSize += sendResult;
			continue;
		}

		// Tries again because it was interrupted by a signal before sending anything
		if (sendResult == -1 && errno == EINTR)
		{
			continue;
		}

		// NOTE: Socket is full so the rest waits in writeBuffer until EPOLLOUT
		if (sendResult == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
			break;
		}

		/**
		 * NOTES:
		 * 	- Real error so the client is gone (EPIPE, ECONNRESET)
		 * 	- Connection is not closed here (SendTo() can run inside SendAll() loop over connections map and erasing would break it)
		 * 	- epoll reports EPOLLERR / EPOLLHUP for the socket and ReadClientSocket closes it
		 */
		m_logger.Error("Failed to send to client socket " + std::to_string(connection.clientSocket) + ", " + std::strerror(errno));
		connection.writeBuffer.clear();

		return;
	}

	// NOTE: Removes what was sent in one erase just like ExtractFrames() from Framer
	connection.writeBuffer.erase(0, sentSize);
}

void SocketServer::SetNonBlockingSocket(int socket)
{
	int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

void SocketServer::WriteClientSocket(int clientSocket)
{
	// Finds connection
	std::unordered_map<int, Connection>::iterator connectionsMapIterator = m_connectionsMap.find(clientSocket);
	if (connectionsMapIterator == m_connectionsMap.end())
	{
		return;
	}

	SendWriteBuffer(connectionsMapIterator->second);
}
