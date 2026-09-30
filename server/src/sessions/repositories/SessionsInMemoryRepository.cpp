#include "sessions/repositories/SessionsInMemoryRepository.h"

// **********
// * PUBLIC *
// **********
SessionsInMemoryRepository::SessionsInMemoryRepository(Logger& logger)
    : m_logger(logger)
{}

Session SessionsInMemoryRepository::Create(const Session& session)
{
    m_sessions.push_back(session);

    return session;
}

std::optional<Session> SessionsInMemoryRepository::Delete(const std::string& id)
{
    for (std::vector<Session>::iterator sessionsIterator = m_sessions.begin(); sessionsIterator != m_sessions.end(); sessionsIterator++)
    {
        if (sessionsIterator->id != id)
        {
            continue;
        }
        
        // Copies session before erasing it as erase destroys the element
        const Session session = *sessionsIterator;
        m_sessions.erase(sessionsIterator);

        return session;
    }

    return std::nullopt;
}

std::optional<Session> SessionsInMemoryRepository::FindById(const std::string& id) const
{
    for (const Session& session : m_sessions)
    {
        if (session.id == id)
        {
            return session;
        }
    }

    return std::nullopt;
}

std::optional<Session> SessionsInMemoryRepository::Refresh(const std::string& id, std::time_t expiredAt)
{
    for (Session& session : m_sessions)
    {
        if (session.id == id)
        {
            session.expiredAt = expiredAt;
            return session;
        }
    }

    return std::nullopt;
}