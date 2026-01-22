#pragma once

#include "ISerializer.h"
#include "socket/RegisteredSocketEvent.h"

class RegisteredSocketEventSerializer: public ISerializer<RegisteredSocketEvent, std::string>
{
public:
    RegisteredSocketEventSerializer() = default;

    std::string Serialize(const RegisteredSocketEvent& LoggedinSocketEvent) override;
};
