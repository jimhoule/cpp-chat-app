#include "SocketServer.h"

#include <cstdlib>
#include <string>

int main()
{
	const int PORT = std::stoi(std::getenv("PORT"));

	SocketServer socket_server;
	socket_server.Init(PORT);
	socket_server.Listen(PORT);
	socket_server.Close();

	return 0;
}
