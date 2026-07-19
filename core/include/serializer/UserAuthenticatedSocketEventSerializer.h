#pragma once

#include "ISerializer.h"
#include "socket/UserAuthenticatedSocketEvent.h"

class UserAuthenticatedSocketEventSerializer: public ISerializer<UserAuthenticatedSocketEvent, std::string>
{
public:
    UserAuthenticatedSocketEventSerializer() = default;

    std::string Serialize(const UserAuthenticatedSocketEvent& userAuthenticatedSocketEvent) override;
};
