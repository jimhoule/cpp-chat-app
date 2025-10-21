#include "SocketServer.h"

#include <array>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

constexpr int MAX_PENDING_CONNECTIONS = 5;
constexpr int BUFFER_SIZE = 1024;

// Getters
sockaddr_in SocketServer::GetAddress() const
{
	return m_address;
}

int SocketServer::GetSocket() const
{
	return m_socket;
}

void SocketServer::Close()
{
	close(m_socket);
}

void SocketServer::Init(const int port)
{
	// Creates socket file descriptor
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Sets server address parameters
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = INADDR_ANY;
	m_address.sin_port = htons(port);

	// Binds the socket to the specified port
	int bind_result = bind(m_socket, reinterpret_cast<sockaddr *>(&m_address), sizeof(m_address));
	if (bind_result < 0)
	{
		perror("bind failed");
		Close();
		exit(EXIT_FAILURE);
	}

	// Sets the server socket to non-blocking mode
	int fcntl_result = fcntl(m_socket, F_SETFL, O_NONBLOCK);
	if (fcntl_result < 0)
	{
		perror("fcntl failed");
		Close();
		exit(EXIT_FAILURE);
	}
}

void SocketServer::Listen(const int port)
{
	// Listens for incoming connections
	int listen_result = listen(m_socket, MAX_PENDING_CONNECTIONS);
	if (listen_result < 0)
	{
		perror("listen failed");
		Close();
		exit(EXIT_FAILURE);
	}

	std::cout << "Server listening on port " << port << " ..." << "\n" << std::endl;

	socklen_t address_length = sizeof(m_address);
	std::string message = "Hello from Server Socket!";
	while (true)
	{
		// Accepts new connections
		int client_socket = accept(m_socket, reinterpret_cast<sockaddr *>(&m_address), &address_length);
		// Handles non-blocking accept errors
		if (client_socket < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
			continue;
		}

		if (client_socket)
		{
			return;
		}

		std::cout << "New connection was accepted for client socket " << client_socket << "\n" << std::endl;

		// Sends message to client
		ssize_t bytes_sent = Send(client_socket, message);
		if (bytes_sent < 0)
		{
			perror("send failed");
		}

		while (client_socket >= 0)
		{
			// Reads response from client
			std::array<char, BUFFER_SIZE> buffer = {0};
			ssize_t read_result = read(client_socket, buffer.data(), BUFFER_SIZE);
			if (read_result >= 0)
			{
				const std::string &client_message(buffer.data());
				std::cout << "Client response: " << client_message << "\n"
				          << "Closing connection to socket " << client_socket << "\n"
				          << std::endl;

				close(client_socket);

				return;
			}
		}
	}
}

ssize_t SocketServer::Send(int client_socket, const std::string &message)
{
	return send(client_socket, message.c_str(), message.length(), 0);
}
