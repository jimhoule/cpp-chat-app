#pragma once

#include "sessions/repositories/ISessionsRepository.h"

#include <vector>

// Forward declarations
class Logger;

class SessionsInMemoryRepository : public ISessionsRepository
{
public:
    SessionsInMemoryRepository(Logger& logger);

    Session Create(const Session& session) override;
    std::optional<Session> Delete(const std::string& id) override;
    std::optional<Session> FindById(const std::string& id) const override;
    std::optional<Session> Refresh(const std::string& id, std::time_t expiredAt) override;

private:
    std::vector<Session> m_sessions = {};

    // NOTE: Borrowed from the module, which owns it and outlives this repository
    Logger& m_logger;
};
