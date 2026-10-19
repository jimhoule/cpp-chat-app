#pragma once

#include "ISerializer.h"
#include "socket/UsersSearchedSocketEvent.h"

class UsersSearchedSocketEventSerializer: public ISerializer<UsersSearchedSocketEvent, std::string>
{
public:
    UsersSearchedSocketEventSerializer() = default;

    std::string Serialize(const UsersSearchedSocketEvent& usersSearchedSocketEvent) override;
};
