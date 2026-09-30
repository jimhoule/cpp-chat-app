#include "sessions/SessionsService.h"

#include "uuid/UuidService.h"

// 7 days in seconds
const std::time_t SESSION_TTL_SECONDS = 7 * 24 * 60 * 60;

// **********
// * PUBLIC *
// **********
SessionsService::SessionsService(std::unique_ptr<ISessionsRepository> sessionsRepository, UuidService& uuidService, Logger& logger)
    : m_sessionsRepository(std::move(sessionsRepository))
    , m_uuidService(uuidService)
    , m_logger(logger)
{}

SessionsService::SessionResult SessionsService::Create(const CreateSessionDto& createSessionDto)
{
    const std::time_t now = std::time(0);

    Session session = {};
    session.id = m_uuidService.Generate();
    session.userId = createSessionDto.userId;
    session.createdAt = now;
    session.expiredAt = GenerateExpirationTime(now);

    SessionResult sessionResult = {};
    sessionResult.data = m_sessionsRepository->Create(session);

    return sessionResult;
}

SessionsService::SessionResult SessionsService::Delete(const DeleteSessionDto& deleteSessionDto)
{
    SessionResult sessionResult = {};
    
    const std::optional<Session> session = m_sessionsRepository->Delete(deleteSessionDto.id);
    if (!session.has_value())
    {
        sessionResult.code = SessionsResultCode::UNKNOWN_SESSION;
        return sessionResult;
    }

    sessionResult.data = session;

    return sessionResult;
}

SessionsService::SessionResult SessionsService::FindById(const FindSessionByIdDto& findSessionByIdDto)
{
    SessionResult sessionResult = {};

    const std::optional<Session> session = m_sessionsRepository->FindById(findSessionByIdDto.id);
    if (!session.has_value())
    {
        return sessionResult;
    }

    const bool isExpired = IsExpired(session.value().expiredAt);
    if (isExpired)
    {
        sessionResult.code = SessionsResultCode::EXPIRED_SESSION;
        return sessionResult;
    }

    sessionResult.data = session;

    return sessionResult;
}

SessionsService::SessionResult SessionsService::Refresh(const RefreshSessionDto& refreshSessionDto)
{
    SessionResult sessionResult = {};

    // NOTE: An expired session cannot be refreshed
    std::optional<Session> existingSession = m_sessionsRepository->FindById(refreshSessionDto.id);
    if (!existingSession.has_value())
    {
        sessionResult.code = SessionsResultCode::UNKNOWN_SESSION;
        return sessionResult;
    }

    const bool isExpired = IsExpired(existingSession.value().expiredAt);
    if (isExpired)
    {
        sessionResult.code = SessionsResultCode::EXPIRED_SESSION;
        return sessionResult;
    }

    const std::time_t now = std::time(0);
    const std::time_t expiredAt = GenerateExpirationTime(now);
    const std::optional<Session> session = m_sessionsRepository->Refresh(refreshSessionDto.id, expiredAt);
    if (!session.has_value())
    {
        sessionResult.code = SessionsResultCode::UNKNOWN_SESSION;
        return sessionResult;
    }

    sessionResult.data = session;

    return sessionResult;
}

std::string SessionsService::ConvertSessionsResultCodeToString(SessionsResultCode sessionsResultCode)
{
    switch (sessionsResultCode)
    {
        case SessionsResultCode::OK:
            return "OK";

        case SessionsResultCode::EXPIRED_SESSION:
            return "EXPIRED SESSION";

        case SessionsResultCode::UNKNOWN_SESSION:
            return "UNKNOWN SESSION";

        default:
            return "Unknown sessions result code";
    }
}

// ***********
// * PRIVATE *
// ***********
std::time_t SessionsService::GenerateExpirationTime(std::time_t now) const
{
    return now + SESSION_TTL_SECONDS;
}

bool SessionsService::IsExpired(std::time_t expiredAt) const
{
    const std::time_t now = std::time(0);

    return expiredAt <= now;
}
