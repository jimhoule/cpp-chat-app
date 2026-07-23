#pragma once

#include "sessions/repositories/ISessionsRepository.h"

#include <vector>

class SessionsInMemoryRepository : public ISessionsRepository
{
public:
    SessionsInMemoryRepository() = default;

    Session Create(const Session& session) override;
    std::optional<Session> FindById(const std::string& id) const override;

private:
    std::vector<Session> m_sessions = {};
};
