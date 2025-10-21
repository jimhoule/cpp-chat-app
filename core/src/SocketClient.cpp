#include "SocketClient.h"

#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

constexpr int BUFFER_SIZE = 1024;

// Getters
int SocketClient::GetSocket() const
{
	return m_socket;
}

void SocketClient::Close()
{
	close(m_socket);
}

void SocketClient::Connect(int server_port, const std::string &server_ip_address)
{
	// Creates socket file descriptor
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Sets the socket to non-blocking mode
	int fcntl_result = fcntl(m_socket, F_SETFL, O_NONBLOCK);
	if (fcntl_result < 0)
	{
		perror("fcntl failed");
		exit(EXIT_FAILURE);
	}

	// Connects to server
	sockaddr_in server_address = sockaddr_in{};
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	server_address.sin_addr.s_addr = inet_addr(server_ip_address.c_str());

	int connect_result = connect(m_socket, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address));
	if (connect_result < 0)
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

void SocketClient::Send(const std::string &message)
{
	// Sends message to server
	send(m_socket, message.c_str(), message.length(), 0);
	std::cout << "Message sent to server\n" << std::endl;

	// Reads response from server
	std::array<char, BUFFER_SIZE> buffer = {0};
	read(m_socket, buffer.data(), BUFFER_SIZE);

	const std::string &server_message(buffer.data());
	std::cout << "Server response: " << server_message << "\n" << std::endl;
}
