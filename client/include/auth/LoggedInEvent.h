#pragma once

#include <string>

struct LoggedInEvent
{
    LoggedInEvent() = default;
    LoggedInEvent(const std::string& sessionId)
        : sessionId(sessionId)
    {}

    std::string sessionId = "";
};
