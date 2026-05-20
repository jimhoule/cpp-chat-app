#pragma once

#include "SocketEvent.h"

#include <string>

struct LoginSocketEventPayload
{
    LoginSocketEventPayload() = default;
    LoginSocketEventPayload(const std::string& Email, const std::string& Password) : Email(Email), Password(Password)
    {}

    std::string Email;
    std::string Password;
};

struct LoginSocketEvent : public SocketEvent<LoginSocketEventPayload>
{
    LoginSocketEvent(LoginSocketEventPayload Payload) :  SocketEvent(SocketEventName::LOGIN, Payload)
    {}
};
