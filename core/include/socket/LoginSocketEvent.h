#pragma once

#include "SocketEvent.h"

#include <string>

struct LoginSocketEventPayload
{
    LoginSocketEventPayload() = default;
    LoginSocketEventPayload(const std::string& email, const std::string& password) : email(email), password(password)
    {}

    std::string email;
    std::string password;
};

struct LoginSocketEvent : public SocketEvent<LoginSocketEventPayload>
{
    LoginSocketEvent(LoginSocketEventPayload payload) :  SocketEvent(SocketEventName::LOGIN, payload)
    {}
};
