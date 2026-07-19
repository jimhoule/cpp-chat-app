#include "auth/AuthHandler.h"
#include "deserializer/LoginSocketEventPayloadDeserializer.h"
#include "deserializer/RegisterSocketEventPayloadDeserializer.h"
#include "messages/MessagesHandler.h"
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
	SocketServer server(PORT);

	// Auth
	AuthService authService;
	AuthHandler authHandler(server, authService);

	server.On(SocketEventName::LOGIN, authHandler.GetLoginHandler());
	server.On(SocketEventName::REGISTER, authHandler.GetRegisterHandler());

	// Messages
	MessagesService messagesService;
	MessagesHandler messagesHandler(server, messagesService);

	server.On(SocketEventName::CREATE_MESSAGE, messagesHandler.GetCreateMessageHandler());

	server.Init();
	server.Listen();
	server.Close();

	return 0;
}
