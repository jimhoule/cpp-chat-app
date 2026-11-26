#include "users/UsersPresenceNotifier.h"

#include "log/Logger.h"
#include "users/UsersService.h"

// **********
// * PUBLIC *
// **********
UsersPresenceNotifier::UsersPresenceNotifier(SocketServer& socketServer, UsersService& usersService, Logger& logger)
    : m_socketServer(socketServer)
    , m_usersService(usersService)
    , m_logger(logger)
{}

void UsersPresenceNotifier::HandleUserBound(const SocketServer::UserBoundContext& context)
{
    // Finds user
    UsersService::FindUserByIdDto findUserByIdDto = {};
    findUserByIdDto.id = context.userId;
    const UsersService::UserResult userResult = m_usersService.FindById(findUserByIdDto);
    if (!userResult.data.has_value())
    {
        m_logger.Error("User online not sent, user " + context.userId + " not found");
        return;
    }

    // Serializes user online socket event
    const UserOnlineSocketEventPayload userOnlineSocketEventPayload(userResult.data.value());
    const UserOnlineSocketEvent userOnlineSocketEvent(userOnlineSocketEventPayload);
    const std::string serializedUserOnlineSocketEvent = m_userOnlineSocketEventSerializer.Serialize(userOnlineSocketEvent);

    // Sends user online socket event to others
    m_socketServer.SendAllExcept(context.clientSocket, serializedUserOnlineSocketEvent);
}

void UsersPresenceNotifier::HandleUserUnbound(const SocketServer::UserUnboundContext& context)
{
    // Serializes user offline socket event
    const UserOfflineSocketEventPayload userOfflineSocketEventPayload(context.userId);
    const UserOfflineSocketEvent userOfflineSocketEvent(userOfflineSocketEventPayload);
    const std::string serializedUserOfflineSocketEvent = m_userOfflineSocketEventSerializer.Serialize(userOfflineSocketEvent);

    // Sends user offline socket event to others (at this point client socket is already unbound so it won't receive anything)
    m_socketServer.SendAll(serializedUserOfflineSocketEvent);
}
