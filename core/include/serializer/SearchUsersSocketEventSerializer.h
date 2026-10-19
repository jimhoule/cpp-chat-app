#pragma once

#include "ISerializer.h"
#include "socket/SearchUsersSocketEvent.h"

class SearchUsersSocketEventSerializer: public ISerializer<SearchUsersSocketEvent, std::string>
{
public:
    SearchUsersSocketEventSerializer() = default;

    std::string Serialize(const SearchUsersSocketEvent& searchUsersSocketEvent) override;
};
