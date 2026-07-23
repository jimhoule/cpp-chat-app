#include "sessions/SessionsModule.h"

#include "sessions/repositories/SessionsInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
SessionsModule::SessionsModule() : m_sessionsService(std::make_unique<SessionsInMemoryRepository>())
{}

SessionsService& SessionsModule::GetService()
{
    return m_sessionsService;
}
