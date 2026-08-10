#include "auth/AuthModule.h"
#include "deserializer/LoginSocketEventPayloadDeserializer.h"
#include "deserializer/RegisterSocketEventPayloadDeserializer.h"
#include "encryption/EncryptionModule.h"
#include "messages/MessagesModule.h"
#include "middlewares/HandleErrors.h"
#include "serializer/LoggedinSocketEventSerializer.h"
#include "serializer/RegisteredSocketEventSerializer.h"
#include "sessions/SessionsModule.h"
#include "socket/SocketServer.h"
#include "users/UsersModule.h"
#include "uuid/UuidModule.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main()
{
	// const int PORT = std::stoi(std::getenv("PORT"));
	const unsigned int PORT = 5000;

	// NOTE: Declared before the server and the error handler that borrow them
	Logger socketServerLogger("SERVER", "core/src/socket/SocketServer");
	Logger handleErrorsLogger("SERVER", "server/src/middlewares/HandleErrors");

	SocketServer server(PORT, socketServerLogger);
	server.OnError(HandleErrors(server, handleErrorsLogger));

	// Modules
	EncryptionModule encryptionModule;
	UuidModule uuidModule;
	SessionsModule sessionsModule(uuidModule.GetService());
	UsersModule usersModule(encryptionModule.GetService(), uuidModule.GetService());
	AuthModule authModule(server, sessionsModule.GetService(), usersModule.GetService());
	MessagesModule messagesModule(server, uuidModule.GetService());

	server.Init();
	server.Listen();
	server.Close();

	return 0;
}
