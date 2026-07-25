#include "sessions/SessionsService.h"

// **********
// * PUBLIC *
// **********
SessionsService::SessionsService(std::unique_ptr<ISessionsRepository> sessionsRepository) : m_sessionsRepository(std::move(sessionsRepository))
{}

Session SessionsService::Create(const CreateSessionDto& createSessionDto)
{
    // 7 days in seconds
    const std::time_t sessionTtlSeconds = 7 * 24 * 60 * 60;

    Session session = {};
    session.id = "uuid." + createSessionDto.userId;
    session.userId = createSessionDto.userId;
    session.createdAt = std::time(0);
    session.expiredAt = std::time(0) + sessionTtlSeconds;

    return m_sessionsRepository->Create(session);
}

std::optional<Session> SessionsService::FindById(const FindSessionByIdDto& findSessionByIdDto)
{
    return m_sessionsRepository->FindById(findSessionByIdDto.id);
}