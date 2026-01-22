#pragma once

#include "ISerializer.h"
#include "socket/RegisterSocketEvent.h"

class RegisterSocketEventSerializer: public ISerializer<RegisterSocketEvent, std::string>
{
public:
    RegisterSocketEventSerializer() = default;

    std::string Serialize(const RegisterSocketEvent& RegisterSocketEvent) override;
};
