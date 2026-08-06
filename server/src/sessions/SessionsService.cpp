#include "sessions/SessionsService.h"

#include "uuid/UuidService.h"

std::string ConvertSessionsResultCodeToString(SessionsResultCode sessionsResultCode)
{
    switch (sessionsResultCode)
    {
        case SessionsResultCode::OK:
            return "OK";

        case SessionsResultCode::SESSION_EXPIRED:
            return "SESSION EXPIRED";

        default:
            return "Unknown sessions result code";
    }
}

// **********
// * PUBLIC *
// **********
SessionsService::SessionsService(std::unique_ptr<ISessionsRepository> sessionsRepository, UuidService& uuidService, Logger& logger)
    : m_sessionsRepository(std::move(sessionsRepository))
    , m_uuidService(uuidService)
    , m_logger(logger)
{}

SessionResult SessionsService::Create(const CreateSessionDto& createSessionDto)
{
    // 7 days in seconds
    const std::time_t sessionTtlSeconds = 7 * 24 * 60 * 60;

    Session session = {};
    session.id = m_uuidService.Generate();
    session.userId = createSessionDto.userId;
    session.createdAt = std::time(0);
    session.expiredAt = std::time(0) + sessionTtlSeconds;

    SessionResult sessionResult = {};
    sessionResult.data = m_sessionsRepository->Create(session);

    return sessionResult;
}

SessionResult SessionsService::FindById(const FindSessionByIdDto& findSessionByIdDto)
{
    SessionResult sessionResult = {};

    std::optional<Session> session = m_sessionsRepository->FindById(findSessionByIdDto.id);
    if (session.value().expiredAt <= std::time(0))
    {
        sessionResult.code = SessionsResultCode::SESSION_EXPIRED;
        return sessionResult;
    }

    sessionResult.data = session;

    return sessionResult;
}