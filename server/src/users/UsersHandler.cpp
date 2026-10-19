#include "users/UsersHandler.h"

#include "users/UsersService.h"

// **********
// * PUBLIC *
// **********
UsersHandler::UsersHandler(SocketServer& socketServer, UsersService& usersService, Logger& logger)
    : m_socketServer(socketServer)
    , m_usersService(usersService)
    , m_logger(logger)
{}

SocketServer::EventHandler UsersHandler::GetSearchUsersHandler()
{
    return [this](const SocketServer::EventContext& context) {
        // Gets search users socket event payload
        const SearchUsersSocketEventPayload searchUsersSocketEventPayload = m_searchUsersSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        // Searches users
        UsersService::SearchUsersDto searchUsersDto = {};
        searchUsersDto.search = searchUsersSocketEventPayload.search;
        const UsersService::UsersResult usersResult = m_usersService.Search(searchUsersDto);
        
        // Serializes users searched socket event
        const UsersSearchedSocketEventPayload usersSearchedSocketEventPayload(usersResult.data);
        const UsersSearchedSocketEvent usersSearchedSocketEvent(usersSearchedSocketEventPayload);
        const std::string serializedUsersSearchedSocketEvent = m_usersSearchedSocketEventSerializer.Serialize(usersSearchedSocketEvent);

        // Sends users searched socket event
        m_socketServer.SendTo(context.clientSocket, serializedUsersSearchedSocketEvent);
    };
}
