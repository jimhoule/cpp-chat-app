#pragma once

#include "log/Logger.h"
#include "users/UsersService.h"

class UsersModule
{
public:
    UsersModule();

    UsersService& GetService();

private:
/**
     * NOTES
     *  - Declaration order is initialization order because these loggers must be declared before the service and the handler that borrow them
     *  - Their order relative to each other does not matter, they are sorted by dependency depth for readability only
     */
    Logger m_usersRepositoryLogger;
    Logger m_usersServiceLogger;
    
    // NOTE: Here the service is not a reference because users service is owned by users module (when users module dies, so does users service)
    UsersService m_usersService;
};
