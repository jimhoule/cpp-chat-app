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
    , m_usersService(std::make_unique<UsersInMemoryRepository>(m_usersRepositoryLogger), encryptionService, uuidService, m_usersServiceLogger)
    , m_usersHandler(socketServer, m_usersService, m_usersHandlerLogger)
{
    socketServer.On(
        SocketEventName::SEARCH_USERS,
        { RequireAuthentication(socketServer) },
        m_usersHandler.GetSearchUsersHandler()
    );
}

UsersService& UsersModule::GetService()
{
    return m_usersService;
}
