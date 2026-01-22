#pragma once

#include "ISerializer.h"
#include "socket/LoggedinSocketEvent.h"

class LoggedinSocketEventSerializer: public ISerializer<LoggedinSocketEvent, std::string>
{
public:
    LoggedinSocketEventSerializer() = default;

    std::string Serialize(const LoggedinSocketEvent& LoggedinSocketEvent) override;
};
