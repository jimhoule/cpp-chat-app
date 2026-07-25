#include "auth/AuthModule.h"
#include "deserializer/LoginSocketEventPayloadDeserializer.h"
#include "deserializer/RegisterSocketEventPayloadDeserializer.h"
#include "messages/MessagesModule.h"
#include "serializer/LoggedinSocketEventSerializer.h"
#include "serializer/RegisteredSocketEventSerializer.h"
#include "sessions/SessionsModule.h"
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

	// Modules
	SessionsModule sessionsModule;
	UsersModule usersModule;
	AuthModule authModule(server, sessionsModule.GetService(), usersModule.GetService());
	MessagesModule messagesModule(server);

	server.Init();
	server.Listen();
	server.Close();

	return 0;
}
