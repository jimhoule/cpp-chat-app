#include "middlewares/RequireAuthentication.h"

#include "exceptions/ExpectedException.h"

SocketServer::EventMiddleware RequireAuthentication(SocketServer& socketServer)
{
    return [&socketServer](SocketServer::EventContext& context) {
        const User* connectionUser = socketServer.GetConnectionUser(context.clientSocket);
        if (connectionUser == nullptr)
        {
            throw ExpectedException(SocketErrorCode::NOT_AUTHENTICATED, "Client socket " + std::to_string(context.clientSocket) + " is not authenticated");
        }

        context.user = connectionUser;
    };
}
