#include "users/UsersApi.h"

#include "socket/SocketClient.h"

// **********
// * PUBLIC *
// **********
UsersApi::UsersApi(SocketClient& socketClient, Logger& logger)
    : m_socketClient(socketClient)
    , m_logger(logger)
{
    SocketClient::EventHandler HandleUsersSearched = [this](const std::string& serializedUsersSearchedSocketEventPayload) {
        const UsersSearchedSocketEventPayload usersSearchedSocketEventPayload = m_usersSearchedSocketEventPayloadDeserializer.Deserialize(serializedUsersSearchedSocketEventPayload);

        // Sends event to subcriptions
        const UsersSearchedEvent usersSearchedEvent(usersSearchedSocketEventPayload.users);
        m_usersSearchedSubject.Notify(usersSearchedEvent);
    };

    m_socketClient.On(SocketEventName::USERS_SEARCHED, this, HandleUsersSearched);
}

UsersApi::~UsersApi()
{
    m_socketClient.Off(SocketEventName::USERS_SEARCHED, this);
}

IObservable<UsersSearchedEvent>& UsersApi::GetUsersSearchedSubject()
{
    return m_usersSearchedSubject;
}

void UsersApi::Search(const SearchUsersParams& searchUsersParams)
{
    // Serializes search users socket event
    const SearchUsersSocketEventPayload searchUsersSocketEventPayload(searchUsersParams.search);
    const SearchUsersSocketEvent searchUsersSocketEvent(searchUsersSocketEventPayload);
    const std::string serializedSearchUsersSocketEvent = m_searchUsersSocketEventSerializer.Serialize(searchUsersSocketEvent);

    // Sends search users socket event
    m_socketClient.Send(serializedSearchUsersSocketEvent);
}
