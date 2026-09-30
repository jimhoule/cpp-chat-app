#pragma once

#include "ISerializer.h"
#include "socket/LogoutSocketEvent.h"

class LogoutSocketEventSerializer: public ISerializer<LogoutSocketEvent, std::string>
{
public:
    LogoutSocketEventSerializer() = default;

    std::string Serialize(const LogoutSocketEvent& logoutSocketEvent) override;
};
