#pragma once

#include "SocketEvent.h"

#include <string>

struct RegisterSocketEventPayload
{
    RegisterSocketEventPayload() = default;
    RegisterSocketEventPayload(const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& Password) : FirstName(FirstName), LastName(LastName), Email(Email), Password(Password)
    {}

    std::string FirstName;
    std::string LastName;
    std::string Email;
    std::string Password;
};

struct RegisterSocketEvent : public SocketEvent<RegisterSocketEventPayload>
{
    RegisterSocketEvent(RegisterSocketEventPayload RegisterSocketEventPayload) :  SocketEvent(SocketEventName::REGISTER, RegisterSocketEventPayload)
    {}
};
