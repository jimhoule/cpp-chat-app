#pragma once

#include "ISerializer.h"
#include "socket/AuthenticatedSocketEvent.h"

class AuthenticatedSocketEventSerializer: public ISerializer<AuthenticatedSocketEvent, std::string>
{
public:
    AuthenticatedSocketEventSerializer() = default;

    std::string Serialize(const AuthenticatedSocketEvent& authenticatedSocketEvent) override;
};
