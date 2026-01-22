#include "auth/AuthHandler.h"
#include "deserializer/LoginSocketEventPayloadDeserializer.h"
#include "deserializer/RegisterSocketEventPayloadDeserializer.h"
#include "serializer/LoggedinSocketEventSerializer.h"
#include "serializer/RegisteredSocketEventSerializer.h"
#include "socket/SocketServer.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main()
{
	// const int PORT = std::stoi(std::getenv("PORT"));
	const unsigned int PORT = 5000;
	SocketServer Server(PORT);

	// Auth
	AuthService AppAuthService;
	AuthHandler AppAuthHandler(Server, AppAuthService);

	Server.On(SocketEventName::LOGIN, AppAuthHandler.GetLoginHandlerFunction());
	Server.On(SocketEventName::REGISTER, AppAuthHandler.GetRegisterHandlerFunction());

	Server.Init();
	Server.Listen();
	Server.Close();

	return 0;
}
