#include "conversations/ConversationsModule.h"

#include "conversations/repositories/ConversationsInMemoryRepository.h"
#include "middlewares/RequireAuthentication.h"
#include "socket/SocketServer.h"

// **********
// * PUBLIC *
// **********
ConversationsModule::ConversationsModule(SocketServer& socketServer, UsersService& usersService, UuidService& uuidService)
    : m_conversationsRepositoryLogger("CONVERSATIONS", "server/src/conversations/repositories/ConversationsInMemoryRepository")
    , m_conversationsServiceLogger("CONVERSATIONS", "server/src/conversations/ConversationsService")
    , m_conversationsHandlerLogger("CONVERSATIONS", "server/src/conversations/ConversationsHandler")
    , m_conversationsService(std::make_unique<ConversationsInMemoryRepository>(m_conversationsRepositoryLogger), usersService, uuidService, m_conversationsServiceLogger)
    , m_conversationsHandler(socketServer, m_conversationsService, m_conversationsHandlerLogger)
{
    socketServer.On(
        SocketEventName::FIND_ALL_OPEN_CONVERSATIONS_BY_USER_ID,
        { RequireAuthentication(socketServer) },
        m_conversationsHandler.GetFindAllOpenConversationsByUserIdHandler()
    );
    socketServer.On(
        SocketEventName::FIND_CONVERSATION_BY_USER_IDS,
        { RequireAuthentication(socketServer) },
        m_conversationsHandler.GetFindConversationByUserIdsHandler()
    );
    socketServer.On(
        SocketEventName::CLOSE_CONVERSATION,
        { RequireAuthentication(socketServer) },
        m_conversationsHandler.GetCloseConversationHandler()
    );
    socketServer.On(
        SocketEventName::OPEN_CONVERSATION,
        { RequireAuthentication(socketServer) },
        m_conversationsHandler.GetOpenConversationHandler()
    );
}

ConversationsService& ConversationsModule::GetService()
{
    return m_conversationsService;
}