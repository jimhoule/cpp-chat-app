#pragma once

#include <string>

struct RegisteredEvent
{
    RegisteredEvent() = default;
    RegisteredEvent(const std::string& sessionId)
        : sessionId(sessionId)
    {}

    std::string sessionId = "";
};
