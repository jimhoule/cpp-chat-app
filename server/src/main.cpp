#include "auth/AuthModule.h"
#include "deserializer/LoginSocketEventPayloadDeserializer.h"
#include "deserializer/RegisterSocketEventPayloadDeserializer.h"
#include "messages/MessagesModule.h"
#include "serializer/LoggedinSocketEventSerializer.h"
#include "serializer/RegisteredSocketEventSerializer.h"
#include "socket/SocketServer.h"
#include "users/UsersModule.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main()
{
	// const int PORT = std::stoi(std::getenv("PORT"));
	const unsigned int PORT = 5000;
	SocketServer server(PORT);

	// Users
	UsersModule usersModule;

	// Auth
	AuthModule authModule(server, usersModule.GetService());

	// Messages
	MessagesModule messagesModule(server);

	server.Init();
	server.Listen();
	server.Close();

	return 0;
}
