#pragma once

#include "SocketEvent.h"

#include <string>

struct RegisterSocketEventPayload
{
    RegisterSocketEventPayload() = default;
    RegisterSocketEventPayload(const std::string& firstName, const std::string& lastName, const std::string& email, const std::string& password) : firstName(firstName), lastName(lastName), email(email), password(password)
    {}

    std::string firstName;
    std::string lastName;
    std::string email;
    std::string password;
};

struct RegisterSocketEvent : public SocketEvent<RegisterSocketEventPayload>
{
    RegisterSocketEvent(RegisterSocketEventPayload payload) :  SocketEvent(SocketEventName::REGISTER, payload)
    {}
};
