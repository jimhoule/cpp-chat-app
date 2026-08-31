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
        SESSION_EXPIRED
    };

    using SessionResult = ServiceResult<SessionsResultCode, std::optional<Session>>;

    struct CreateSessionDto
    {
        std::string userId;
    };

    struct FindSessionByIdDto
    {
        std::string id;
    };

    SessionsService(std::unique_ptr<ISessionsRepository> usersRepository, UuidService& uuidService, Logger& logger);

    SessionResult Create(const CreateSessionDto& createSessionDto);
    SessionResult FindById(const FindSessionByIdDto& findSessionByIdDto);
    std::string ConvertSessionsResultCodeToString(SessionsResultCode sessionsResultCode);

private:
    std::unique_ptr<ISessionsRepository> m_sessionsRepository = nullptr;

    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;
};
