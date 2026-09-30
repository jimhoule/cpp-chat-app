#pragma once

#include "results/ServiceResult.h"
#include "sessions/repositories/ISessionsRepository.h"

// Forward declarations
class Logger;
class UuidService;

class SessionsService
{
public:
    enum class SessionsResultCode
    {
        OK,
        EXPIRED_SESSION,
        UNKNOWN_SESSION
    };

    using SessionResult = ServiceResult<SessionsResultCode, std::optional<Session>>;

    struct CreateSessionDto
    {
        std::string userId;
    };

    struct DeleteSessionDto
    {
        std::string id;
    };

    struct FindSessionByIdDto
    {
        std::string id;
    };

    struct RefreshSessionDto
    {
        std::string id;
    };

    SessionsService(std::unique_ptr<ISessionsRepository> usersRepository, UuidService& uuidService, Logger& logger);

    SessionResult Create(const CreateSessionDto& createSessionDto);
    SessionResult Delete(const DeleteSessionDto& deleteSessionDto);
    SessionResult FindById(const FindSessionByIdDto& findSessionByIdDto);
    SessionResult Refresh(const RefreshSessionDto& refreshSessionDto);
    std::string ConvertSessionsResultCodeToString(SessionsResultCode sessionsResultCode);

private:
    std::unique_ptr<ISessionsRepository> m_sessionsRepository = nullptr;

    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;

    std::time_t GenerateExpirationTime(std::time_t now) const;
    bool IsExpired(std::time_t expiredAt) const;
};
