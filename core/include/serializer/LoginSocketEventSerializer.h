#pragma once

#include "ISerializer.h"
#include "socket/LoginSocketEvent.h"

class LoginSocketEventSerializer: public ISerializer<LoginSocketEvent, std::string>
{
public:
    LoginSocketEventSerializer() = default;

    std::string Serialize(const LoginSocketEvent& loginSocketEvent) override;
};
