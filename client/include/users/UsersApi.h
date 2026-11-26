#pragma once

#include "deserializer/UsersSearchedSocketEventPayloadDeserializer.h"
#include "users/UsersSearchedEvent.h"
#include "observer/IObservable.h"
#include "observer/Subject.h"
#include "serializer/SearchUsersSocketEventSerializer.h"
#include "socket/SocketClient.h"

// Forward declarations
class Logger;

class UsersApi
{
public:
    struct SearchUsersParams
    {
        std::string search;
    };

    UsersApi(SocketClient& socketClient, Logger& logger);
    ~UsersApi();

    // NOTE: Exposes observable view only so callers cannot notify
    IObservable<UsersSearchedEvent>& GetUsersSearchedSubject();

    void Search(const SearchUsersParams& searchUsersParams);
    
private:
    SocketClient& m_socketClient;

    Subject<UsersSearchedEvent> m_usersSearchedSubject = {};

    SearchUsersSocketEventSerializer m_searchUsersSocketEventSerializer = {};
    UsersSearchedSocketEventPayloadDeserializer m_usersSearchedSocketEventPayloadDeserializer = {};

    Logger& m_logger;

    void HandleUsersSearched(const SocketClient::EventContext& context);
};
