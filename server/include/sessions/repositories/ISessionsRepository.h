#pragma once

#include "models/Session.h"

#include "optional"
#include <string>

class ISessionsRepository
{
public:
    // NOTE: Explicitly default the constructor (Optional but recommended for clarity)
    ISessionsRepository() = default;

    // NOTE: ALWAYS provide a public virtual destructor
    virtual ~ISessionsRepository() = default;

    // Disables copying and moving to prevent slicing
    ISessionsRepository(const ISessionsRepository&) = delete;
    ISessionsRepository& operator=(const ISessionsRepository&) = delete;

    virtual Session Create(const Session& session) = 0;
    virtual std::optional<Session> FindById(const std::string& id) const = 0;
};
