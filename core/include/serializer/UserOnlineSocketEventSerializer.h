#pragma once

#include "ISerializer.h"
#include "socket/UserOnlineSocketEvent.h"

class UserOnlineSocketEventSerializer: public ISerializer<UserOnlineSocketEvent, std::string>
{
public:
    UserOnlineSocketEventSerializer() = default;

    std::string Serialize(const UserOnlineSocketEvent& userOnlineSocketEvent) override;
};
