#pragma once

#include "socket/SocketEvent.h"

#include <string>

struct SearchUsersSocketEventPayload
{
    SearchUsersSocketEventPayload() = default;
    SearchUsersSocketEventPayload(const std::string& search)
        : search(search)
    {}

    std::string search;
};

struct SearchUsersSocketEvent : public SocketEvent<SearchUsersSocketEventPayload>
{
    SearchUsersSocketEvent(SearchUsersSocketEventPayload payload) :  SocketEvent(SocketEventName::SEARCH_USERS, payload)
    {}
};