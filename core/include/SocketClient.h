#include <string>

class SocketClient
{
  public:
	SocketClient() = default;

	// Getters
	[[nodiscard]] int GetSocket() const;

	void Close();
	void Connect(int server_port, const std::string &server_ip_address);
	void Send(const std::string &message);

  private:
	int m_socket = 0;
};
