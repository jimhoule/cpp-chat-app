#include "sessions/repositories/SessionsInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
Session SessionsInMemoryRepository::Create(const Session& session)
{
    m_sessions.push_back(session);

    return session;
}

std::optional<Session> SessionsInMemoryRepository::FindById(const std::string& id) const
{
    for (const Session& session : m_sessions)
    {
        if (session.id == id) return session;
    }

    return std::nullopt;
}