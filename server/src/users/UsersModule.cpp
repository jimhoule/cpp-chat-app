#include "users/UsersModule.h"

#include "users/repositories/UsersInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
UsersModule::UsersModule()
    : m_usersRepositoryLogger("USERS", "server/src/users/repositories/UsersInMemoryRepository")
    , m_usersServiceLogger("USERS", "server/src/users/UserssService")
    , m_usersService(std::make_unique<UsersInMemoryRepository>(m_usersRepositoryLogger), m_usersServiceLogger)
{}

UsersService& UsersModule::GetService()
{
    return m_usersService;
}
