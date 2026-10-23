#include "socket/SocketClient.h"

#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

constexpr int BUFFER_SIZE = 4096;
/**
 * NOTES:
 * 	- Sending to a server that is gone must return an error instead of killing the client (SIGPIPE)
 * 	- Linux has MSG_NOSIGNAL per send()
 * 	- MacOS has SO_NOSIGPIPE per socket (set in Connect())
 */
#ifdef MSG_NOSIGNAL
constexpr int SEND_FLAGS = MSG_NOSIGNAL;
#else
constexpr int SEND_FLAGS = 0;
#endif

// **********
// * PUBLIC *
// **********
void SocketClient::Close()
{
	close(m_connection.socket);
	// NOTE: Resets everything tied to old socket's byte stream so a reconnect starts clean
	m_connection = {};
}

void SocketClient::Connect(int serverPort, const std::string &serverIpAddress)
{
	// Creates socket file descriptor
	m_connection.socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_connection.socket == -1)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Sets the socket to non-blocking mode
	int fcntlResult = fcntl(m_connection.socket, F_SETFL, O_NONBLOCK);
	if (fcntlResult < 0)
	{
		perror("fcntl failed");
		exit(EXIT_FAILURE);
	}

	/**
	 * NOTES:
	 * 	- On macos sending to a server that is gone kills the client (SIGPIPE)
	 * 	- SO_NOSIGPIPE makes send() return an error (EPIPE) instead
	 */
	#ifdef SO_NOSIGPIPE
		int noSigPipe = 1;
		setsockopt(m_connection.socket, SOL_SOCKET, SO_NOSIGPIPE, &noSigPipe, sizeof(noSigPipe));
	#endif

	// Connects to server
	sockaddr_in serverAddress = sockaddr_in{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);
	serverAddress.sin_addr.s_addr = inet_addr(serverIpAddress.c_str());

	int connectResult = connect(m_connection.socket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));
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

void SocketClient::Off(SocketEventName socketEventName, EventHandlerOwner owner)
{
	EventHandlersMap::iterator handlersMapIterator = m_handlersMap.find(socketEventName);
	if (handlersMapIterator == m_handlersMap.end())
	{
		std::cout << "No handler registered for socket event " << socketEventName << std::endl;
		return;
	}

	EventHandlerEntries& handlerEntries = handlersMapIterator->second;
	for (std::size_t index = 0; index < handlerEntries.size(); index++)
	{
		const EventHandlerEntry& handlerEntry = handlerEntries[index];
		if (handlerEntry.owner == owner)
		{
			handlerEntries.erase(handlerEntries.begin() + index);
			return;
		}
	}
}

void SocketClient::On(SocketEventName socketEventName, EventHandlerOwner owner, const EventHandler& handler)
{
    EventHandlerEntries& handlerEntries = m_handlersMap[socketEventName];
	/**
	 * NOTES:
	 * 	- Checks if owner already registered the event
	 * 	- A owner cannot register the same event twice
	 */
	for (std::size_t index = 0; index < handlerEntries.size(); index++)
	{
		const EventHandlerEntry& handlerEntry = handlerEntries[index];
		if (handlerEntry.owner == owner)
		{
			return;
		}
	}

	EventHandlerEntry handlerEntry = {};
	handlerEntry.handler = handler;
	handlerEntry.owner = owner;
	handlerEntries.push_back(handlerEntry);
}

void SocketClient::Send(const std::string &serializedSocketEvent)
{
	if (m_connection.socket == -1)
	{
		return;
	}

	/**
	 * NOTES:
	 *	- Never waits so a slow server cannot freeze the render loop
	 *	- Send what it can now and SendWriteBuffer() sends the rest on the next frame
	 */
	m_connection.writeBuffer += Framer::Encode(serializedSocketEvent);
	SendWriteBuffer();
}

void SocketClient::Update()
{
	Read();
	SendWriteBuffer();
}

// ***********
// * PRIVATE *
// ***********
void SocketClient::DispatchSocketEvent(const std::string& serializedSocketEvent)
{
	SocketEvent<std::string> socketEvent = m_socketEventDeserializer.Deserialize(serializedSocketEvent);

	// NOTE: find, not operator[], which would insert an empty handler entries vector for an unknown event
	EventHandlersMap::iterator handlersMapIterator = m_handlersMap.find(socketEvent.name);
	if (handlersMapIterator == m_handlersMap.end())
	{
		std::cout << "No handler registered for socket event " << socketEvent.name << std::endl;
		return;
	}

	EventHandlerEntries& handlerEntries = handlersMapIterator->second;
	for (const EventHandlerEntry& handlerEntry : handlerEntries)
	{
		const EventHandler& HandleSocketEvent = handlerEntry.handler;
		HandleSocketEvent(socketEvent.payload);
	}
}

void SocketClient::Read()
{
	if (m_connection.socket == -1)
	{
		return;
	}

	/**
	 * NOTES:
	 * 	- Reads until socket is empty
	 *	- One read() per Read() call would take several render frames to receive a big payload
	 */
	std::array<char, BUFFER_SIZE> chunk = {};
	while (true)
	{
		// Reads response from server
		ssize_t readResult = read(m_connection.socket, chunk.data(), BUFFER_SIZE);

		// Adds chunk to frame
		if (readResult > 0)
		{
			m_connection.framer.Append(chunk.data(), readResult);
			continue;
		}

		// NOTE: Server is closed
		if (readResult == 0)
		{
			std::cout << "Server closed" << std::endl;
			Close();
			
			return;
		}

		// NOTE: EAGAIN means nothing more to read for now
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			break;
		}

		// NOTE: Real error and also how a failed connect shows up (ECONNREFUSED)
		perror("read failed");
		Close();

		return;
	}

	/**
	 * NOTES:
	 * 	- Can return several events (sent close together) or none (payload not fully arrived yet)
	 * 	- Throws when stream is out of sync (ex: server without framing) and if left uncaught it would
	 * 	  reach the main loop and terminate the client
	 * 	- The try only covers ExtractFrames() so an std::length_error thrown by a handler is not reported
	 * 	  as an invalid frame
	 */
	std::vector<std::string> serializedSocketEvents = {};
	try
	{
		serializedSocketEvents = m_connection.framer.ExtractFrames();
	}
	catch(const std::length_error& exception)
	{
		std::cout << "Invalid frame from server, " << exception.what() << std::endl;
		Close();

		return;
	}

	for (const std::string& serializedSocketEvent : serializedSocketEvents)
	{
		DispatchSocketEvent(serializedSocketEvent);
	}
}

void SocketClient::SendWriteBuffer()
{
	if (m_connection.socket == -1 || m_connection.writeBuffer.empty())
	{
		return;
	}

	// NOTE: How many bytes of write buffer have been sent so far
	std::size_t sentSize = 0;
	while (sentSize < m_connection.writeBuffer.size())
	{
		std::size_t size = m_connection.writeBuffer.size() - sentSize;
		ssize_t sendResult = send(m_connection.socket, m_connection.writeBuffer.data() + sentSize, size, SEND_FLAGS);

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

		/**
		 * NOTES:
		 * 	- Socket is full (EAGAIN) or still connecting (ENOTCONN for macos and EAGAIN for linux)
		 * 	- The rest waits in the write buffer and the next frame's SendWriteBuffer() tries again
		 */
		if (sendResult == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == ENOTCONN))
		{
			break;
		}

		// NOTE: Real error so the server is gone (EPIPE, ECONNRESET)
		perror("Send failed");
		Close();

		return;
	}

	// NOTE: Removes what was sent in one erase just like SendWriteBuffer() from SocketServer
	m_connection.writeBuffer.erase(0, sentSize);
}
