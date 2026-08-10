#include "messages/MessagesModule.h"

#include "messages/repositories/MessagesInMemoryRepository.h"
#include "middlewares/RequireAuthentication.h"
#include "socket/SocketServer.h"

// **********
// * PUBLIC *
// **********
MessagesModule::MessagesModule(SocketServer& socketServer, UuidService& uuidService)
    : m_messagesRepositoryLogger("MESSAGES", "server/src/messages/repositories/MessagesInMemoryRepository")
    , m_messagesServiceLogger("MESSAGES", "server/src/messages/MessagesService")
    , m_messagesHandlerLogger("MESSAGES", "server/src/messages/MessagesHandler")
    , m_messagesService(std::make_unique<MessagesInMemoryRepository>(m_messagesRepositoryLogger), uuidService, m_messagesServiceLogger)
    , m_messagesHandler(socketServer, m_messagesService, m_messagesHandlerLogger)
{
    socketServer.On(
        SocketEventName::CREATE_MESSAGE,
        { RequireAuthentication(socketServer) },
        m_messagesHandler.GetCreateMessageHandler()
    );
}

MessagesService& MessagesModule::GetService()
{
    return m_messagesService;
}