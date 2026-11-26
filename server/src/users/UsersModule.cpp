#include "users/UsersModule.h"

#include "middlewares/RequireAuthentication.h"
#include "socket/SocketServer.h"
#include "users/repositories/UsersInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
UsersModule::UsersModule(SocketServer& socketServer, EncryptionService& encryptionService, UuidService& uuidService)
    : m_usersRepositoryLogger("USERS", "server/src/users/repositories/UsersInMemoryRepository")
    , m_usersServiceLogger("USERS", "server/src/users/UsersService")
    , m_usersHandlerLogger("USERS", "server/src/users/UsersHandler")
    , m_usersPresenceNotifierLogger("USERS", "server/src/users/UsersPresenceNotifier")
    , m_usersService(std::make_unique<UsersInMemoryRepository>(m_usersRepositoryLogger), encryptionService, uuidService, m_usersServiceLogger)
    , m_usersHandler(socketServer, m_usersService, m_usersHandlerLogger)
    , m_usersPresenceNotifier(socketServer, m_usersService, m_usersPresenceNotifierLogger)
{
    socketServer.On(
        SocketEventName::SEARCH_USERS,
        { RequireAuthentication(socketServer) },
        m_usersHandler,
        &UsersHandler::HandleSearchUsers
    );

    socketServer.OnUserBound(m_usersPresenceNotifier, &UsersPresenceNotifier::HandleUserBound);
    socketServer.OnUserUnbound(m_usersPresenceNotifier, &UsersPresenceNotifier::HandleUserUnbound);
}

UsersService& UsersModule::GetService()
{
    return m_usersService;
}
