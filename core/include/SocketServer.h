#include <arpa/inet.h>
#include <string>

class SocketServer
{
  public:
	SocketServer() = default;

	// Getters
	[[nodiscard]] sockaddr_in GetAddress() const;
	[[nodiscard]] int GetSocket() const;

	void Close();
	void Init(const int port);
	void Listen(const int port);
	ssize_t Send(int client_socket, const std::string &message);

  private:
	sockaddr_in m_address = sockaddr_in{};
	int m_socket = 0;
};
