#pragma once

#include "ISerializer.h"
#include "socket/AuthenticateSocketEvent.h"

class AuthenticateSocketEventSerializer: public ISerializer<AuthenticateSocketEvent, std::string>
{
public:
    AuthenticateSocketEventSerializer() = default;

    std::string Serialize(const AuthenticateSocketEvent& authenticateSocketEvent) override;
};
