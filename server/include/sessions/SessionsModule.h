#pragma once

#include "sessions/SessionsService.h"

class SessionsModule
{
public:
    SessionsModule();

    SessionsService& GetService();

private:
    // NOTE: Here the service is not a reference because sessions service is owned by sessions module (when sessions module dies, so does sessions service)
    SessionsService m_sessionsService;
};
