#pragma once

#include "ISerializer.h"
#include "socket/LoggedoutSocketEvent.h"

class LoggedoutSocketEventSerializer: public ISerializer<LoggedoutSocketEvent, std::string>
{
public:
    LoggedoutSocketEventSerializer() = default;

    std::string Serialize(const LoggedoutSocketEvent& loggedoutSocketEvent) override;
};
