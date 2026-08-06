#pragma once

#include "log/Logger.h"
#include "sessions/SessionsService.h"

// Forward declarations
class UuidService;

class SessionsModule
{
public:
    SessionsModule(UuidService& uuidService);

    SessionsService& GetService();

private:
    /**
     * NOTES
     *  - Declaration order is initialization order because these loggers must be declared before the service and the handler that borrow them
     *  - Their order relative to each other does not matter, they are sorted by dependency depth for readability only
     */
    Logger m_sessionsRepositoryLogger;
    Logger m_sessionsServiceLogger;

    // NOTE: Here the service is not a reference because sessions service is owned by sessions module (when sessions module dies, so does sessions service)
    SessionsService m_sessionsService;
};
