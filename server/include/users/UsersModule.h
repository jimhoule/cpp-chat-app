#pragma once

#include "users/UsersService.h"

class UsersModule
{
public:
    UsersModule();

    UsersService& GetService();

private:
    // NOTE: Here the service is not a reference because users service is owned by users module (when users module dies, so does users service)
    UsersService m_usersService;
};
