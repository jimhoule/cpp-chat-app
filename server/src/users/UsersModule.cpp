#include "users/UsersModule.h"

#include "users/repositories/UsersInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
UsersModule::UsersModule() : m_usersService(std::make_unique<UsersInMemoryRepository>())
{}

UsersService& UsersModule::GetService()
{
    return m_usersService;
}
