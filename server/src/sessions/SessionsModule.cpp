#include "sessions/SessionsModule.h"

#include "sessions/repositories/SessionsInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
SessionsModule::SessionsModule()
    : m_sessionsRepositoryLogger("SESSIONS", "server/src/sessions/repositories/SessionsInMemoryRepository")
    , m_sessionsServiceLogger("SESSIONS", "server/src/sessions/SessionsService")
    , m_sessionsService(std::make_unique<SessionsInMemoryRepository>(m_sessionsRepositoryLogger), m_sessionsServiceLogger)
{}

SessionsService& SessionsModule::GetService()
{
    return m_sessionsService;
}
