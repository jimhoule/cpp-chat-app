#include "messages/MessagesModule.h"

#include "messages/repositories/MessagesInMemoryRepository.h"
#include "middlewares/RequireAuthentication.h"
#include "socket/SocketServer.h"

// **********
// * PUBLIC *
// **********
MessagesModule::MessagesModule(SocketServer& socketServer, ConversationsService& conversationsService, UuidService& uuidService)
    : m_messagesRepositoryLogger("MESSAGES", "server/src/messages/repositories/MessagesInMemoryRepository")
    , m_messagesServiceLogger("MESSAGES", "server/src/messages/MessagesService")
    , m_messagesHandlerLogger("MESSAGES", "server/src/messages/MessagesHandler")
    , m_messagesService(std::make_unique<MessagesInMemoryRepository>(m_messagesRepositoryLogger), conversationsService, uuidService, m_messagesServiceLogger)
    , m_messagesHandler(socketServer, m_messagesService, m_messagesHandlerLogger)
{
    socketServer.On(
        SocketEventName::CREATE_MESSAGE,
        { RequireAuthentication(socketServer) },
        m_messagesHandler,
        &MessagesHandler::HandleCreateMessage
    );
    socketServer.On(
        SocketEventName::FIND_ALL_OPEN_MESSAGES_READ_STATES_BY_USER_ID,
        { RequireAuthentication(socketServer) },
        m_messagesHandler,
        &MessagesHandler::HandleFindAllOpenMessagesReadStatesByUserId
    );
    socketServer.On(
        SocketEventName::FIND_MESSAGES_PAGE_BY_CONVERSATION_ID,
        { RequireAuthentication(socketServer) },
        m_messagesHandler,
        &MessagesHandler::HandleFindMessagesPageByConversationId
    );
    socketServer.On(
        SocketEventName::FIND_MESSAGES_READ_STATE_BY_CONVERSATION_ID,
        { RequireAuthentication(socketServer) },
        m_messagesHandler,
        &MessagesHandler::HandleFindMessagesReadStateByConversationId
    );
    socketServer.On(
        SocketEventName::MARK_MESSAGES_READ,
        { RequireAuthentication(socketServer) },
        m_messagesHandler,
        &MessagesHandler::HandleMarkMessagesRead
    );
}

MessagesService& MessagesModule::GetService()
{
    return m_messagesService;
}