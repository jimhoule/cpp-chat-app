#pragma once

#include "sessions/repositories/ISessionsRepository.h"

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
    SessionsService(std::unique_ptr<ISessionsRepository> usersRepository);

    Session Create(const CreateSessionDto& createSessionDto);
    std::optional<Session> FindById(const FindSessionByIdDto& findSessionByIdDto);

private:
    std::unique_ptr<ISessionsRepository> m_sessionsRepository = nullptr;
};
