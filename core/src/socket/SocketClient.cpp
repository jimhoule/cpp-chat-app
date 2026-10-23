#include "socket/SocketClient.h"

#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

constexpr int BUFFER_SIZE = 4096;

// **********
// * PUBLIC *
// **********
void SocketClient::Close()
{
	close(m_socket);
	m_socket = -1;
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

void SocketClient::Read()
{
	if (m_socket < 0)
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
		ssize_t readResult = read(m_socket, chunk.data(), BUFFER_SIZE);

		// Adds chunk to frame
		if (readResult > 0)
		{
			m_framer.Append(chunk.data(), readResult);
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

		perror("read failed");

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
		serializedSocketEvents = m_framer.ExtractFrames();
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

void SocketClient::Send(const std::string &serializedSocketEvent)
{
	const std::string frame = Framer::Encode(serializedSocketEvent);
	send(m_socket, frame.data(), frame.size(), 0);
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
