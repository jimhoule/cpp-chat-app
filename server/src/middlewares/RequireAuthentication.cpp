#include "middlewares/RequireAuthentication.h"

#include "exceptions/ExpectedException.h"

#include <string>

SocketServer::EventMiddleware RequireAuthentication(SocketServer& socketServer)
{
    return [&socketServer](SocketServer::EventContext& context) {
        const std::string* connectionUserId = socketServer.GetConnectionUserId(context.clientSocket);
        if (connectionUserId == nullptr)
        {
            throw ExpectedException(SocketErrorCode::NOT_AUTHENTICATED, "Client socket " + std::to_string(context.clientSocket) + " is not authenticated");
        }

        context.userId = connectionUserId;
    };
}
