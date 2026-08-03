#include "middlewares/HandleErrors.h"

#include "exceptions/ExpectedException.h"
#include "json/JsonException.h"
#include "log/Logger.h"
#include "serializer/ErrorSocketEventSerializer.h"
#include "socket/SocketErrorCode.h"
#include "socket/SocketServer.h"

SocketServerEventErrorHandler HandleErrors(SocketServer& socketServer, Logger& logger)
{
    return [&socketServer, &logger](const std::exception& exception, SocketEventContext& context) {
        SocketErrorCode socketErrorCode = SocketErrorCode::INTERNAL;

        const ExpectedException* expectedException = dynamic_cast<const ExpectedException*>(&exception);
        const JsonException* jsonException = dynamic_cast<const JsonException*>(&exception);

        // NOTE: An ExpectedException was thrown on purpose, the client is at fault and the server is fine
        if (expectedException != nullptr)
        {
            socketErrorCode = expectedException->GetSocketErrorCode();
            logger.Warning("Rejected event from client socket " + std::to_string(context.clientSocket) + ", " + expectedException->what());
        }
        else if (jsonException != nullptr)
        {
            socketErrorCode = SocketErrorCode::INVALID_PAYLOAD;
            logger.Warning("Rejected malformed event from client socket " + std::to_string(context.clientSocket) + ", " + jsonException->what());
        }
        // NOTE: Nobody anticipated this one, the server is at fault
        else
        {
            logger.Error("Failed event from client socket " + std::to_string(context.clientSocket) + ", " + exception.what());
        }

        // Sends error socket event
        ErrorSocketEventSerializer errorSocketEventSerializer = {};
        const ErrorSocketEventPayload errorSocketEventPayload(socketErrorCode);
        const ErrorSocketEvent errorSocketEvent(errorSocketEventPayload);
        socketServer.SendTo(context.clientSocket, errorSocketEventSerializer.Serialize(errorSocketEvent));
    };
}
