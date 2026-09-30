#pragma once

#include "ISerializer.h"
#include "socket/UserOfflineSocketEvent.h"

class UserOfflineSocketEventSerializer: public ISerializer<UserOfflineSocketEvent, std::string>
{
public:
    UserOfflineSocketEventSerializer() = default;

    std::string Serialize(const UserOfflineSocketEvent& userOfflineSocketEvent) override;
};
