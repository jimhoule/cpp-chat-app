#pragma once

#include "sessions/repositories/ISessionsRepository.h"

// Forward declarations
class Logger;

struct CreateSessionDto
{
    std::string userId;
};

struct FindSessionByIdDto
{
    std::string id;
};

class SessionsService
{
public:
    SessionsService(std::unique_ptr<ISessionsRepository> usersRepository, Logger& logger);

    Session Create(const CreateSessionDto& createSessionDto);
    std::optional<Session> FindById(const FindSessionByIdDto& findSessionByIdDto);

private:
    std::unique_ptr<ISessionsRepository> m_sessionsRepository = nullptr;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;
};
