#include "conversations/ConversationsHandler.h"

#include "exceptions/ExpectedException.h"

// **********
// * PUBLIC *
// **********
ConversationsHandler::ConversationsHandler(SocketServer& socketServer, ConversationsService& conversationsService, Logger& logger)
    : m_socketServer(socketServer)
    , m_conversationsService(conversationsService)
    , m_logger(logger)
{}

SocketServer::EventHandler ConversationsHandler::GetFindAllOpenConversationsByUserIdHandler()
{
    return [this](const SocketServer::EventContext& context) {
        // NOTE: The user id comes from the authenticated context, never from the payload
        ConversationsService::FindAllOpenConversationsByUserIdDto findAllOpenConversationsByUserIdDto = {};
        findAllOpenConversationsByUserIdDto.userId = context.user->id;
        const ConversationsService::ConversationsResult conversationsResult = m_conversationsService.FindAllOpenByUserId(findAllOpenConversationsByUserIdDto);
        if (conversationsResult.code != ConversationsService::ConversationsResultCode::OK)
        {
            const std::string conversationsResultCodeString = m_conversationsService.ConvertConversationsResultCodeToString(conversationsResult.code);
            const SocketErrorCode socketErrorCode = ConvertConversationsResultCodeToSocketErrorCode(conversationsResult.code);
            throw ExpectedException(
                socketErrorCode,
                "Find all open conversations failed for user " + context.user->id + ", " + conversationsResultCodeString
            );
        }

        const AllOpenConversationsFoundByUserIdSocketEventPayload allOpenConversationsFoundByUserIdSocketEventPayload(conversationsResult.data);
        const AllOpenConversationsFoundByUserIdSocketEvent allOpenConversationsFoundByUserIdSocketEvent(allOpenConversationsFoundByUserIdSocketEventPayload);
        const std::string serializedOpenConversationsFoundSocketEvent = m_allOpenConversationsFoundByUserIdSocketEventSerializer.Serialize(allOpenConversationsFoundByUserIdSocketEvent);

        m_socketServer.SendTo(context.clientSocket, serializedOpenConversationsFoundSocketEvent);
    };
}

SocketServer::EventHandler ConversationsHandler::GetFindConversationByUserIdsHandler()
{
    return [this](const SocketServer::EventContext& context) {
        const FindConversationByUserIdsSocketEventPayload findConversationByUserIdsSocketEventPayload = m_findConversationByUserIdsSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        ConversationsService::FindConversationByUserIdsDto findConversationByUserIdsDto = {};
        // NOTE: The initiator comes from the authenticated context, never from the payload
        findConversationByUserIdsDto.initiatorUserId = context.user->id;
        findConversationByUserIdsDto.userIds = findConversationByUserIdsSocketEventPayload.userIds;
        const ConversationsService::ConversationResult conversationResult = m_conversationsService.FindByUserIds(findConversationByUserIdsDto);
        if (conversationResult.code != ConversationsService::ConversationsResultCode::OK)
        {
            const std::string conversationsResultCodeString = m_conversationsService.ConvertConversationsResultCodeToString(conversationResult.code);
            const SocketErrorCode socketErrorCode = ConvertConversationsResultCodeToSocketErrorCode(conversationResult.code);
            throw ExpectedException(
                socketErrorCode,
                "Find conversation by user ids failed for user " + context.user->id + ", " + conversationsResultCodeString
            );
        }

        // NOTE: An empty conversation is a normal answer so optional stays wrapped
        const ConversationFoundByUserIdsSocketEventPayload conversationFoundByUserIdsSocketEventPayload(conversationResult.data);
        const ConversationFoundByUserIdsSocketEvent conversationFoundByUserIdsSocketEvent(conversationFoundByUserIdsSocketEventPayload);
        const std::string serializedConversationFoundByUserIdsSocketEvent = m_conversationFoundByUserIdsSocketEventSerializer.Serialize(conversationFoundByUserIdsSocketEvent);

        m_socketServer.SendTo(context.clientSocket, serializedConversationFoundByUserIdsSocketEvent);
    };
}

SocketServer::EventHandler ConversationsHandler::GetCloseConversationHandler()
{
    return [this](const SocketServer::EventContext& context) {
        const CloseConversationSocketEventPayload closeConversationSocketEventPayload = m_closeConversationSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        ConversationsService::CloseConversationDto closeConversationDto = {};
        // NOTE: User id comes from the authenticated context, never from the payload
        closeConversationDto.userId = context.user->id;
        closeConversationDto.conversationId = closeConversationSocketEventPayload.conversationId;
        const ConversationsService::ConversationResult conversationResult = m_conversationsService.Close(closeConversationDto);
        if (conversationResult.code != ConversationsService::ConversationsResultCode::OK)
        {
            const std::string conversationsResultCodeString = m_conversationsService.ConvertConversationsResultCodeToString(conversationResult.code);
            const SocketErrorCode socketErrorCode = ConvertConversationsResultCodeToSocketErrorCode(conversationResult.code);
            throw ExpectedException(
                socketErrorCode,
                "Close conversation for user " + context.user->id + ", " + conversationsResultCodeString
            );
        }

        const ConversationClosedSocketEventPayload conversationClosedSocketEventPayload(conversationResult.data.value());
        const ConversationClosedSocketEvent conversationClosedSocketEvent(conversationClosedSocketEventPayload);
        const std::string serializedConversationClosedSocketEvent = m_conversationClosedSocketEventSerializer.Serialize(conversationClosedSocketEvent);

        m_socketServer.SendTo(context.clientSocket, serializedConversationClosedSocketEvent);
    };
}

SocketServer::EventHandler ConversationsHandler::GetOpenConversationHandler()
{
    return [this](const SocketServer::EventContext& context) {
        const OpenConversationSocketEventPayload openConversationSocketEventPayload = m_openConversationSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        ConversationsService::OpenConversationDto openConversationDto = {};
        // NOTE: The initiator comes from the authenticated context, never from the payload
        openConversationDto.initiatorUserId = context.user->id;
        openConversationDto.userIds = openConversationSocketEventPayload.userIds;
        const ConversationsService::ConversationResult conversationResult = m_conversationsService.Open(openConversationDto);
        if (conversationResult.code != ConversationsService::ConversationsResultCode::OK)
        {
            const std::string conversationsResultCodeString = m_conversationsService.ConvertConversationsResultCodeToString(conversationResult.code);
            const SocketErrorCode socketErrorCode = ConvertConversationsResultCodeToSocketErrorCode(conversationResult.code);
            throw ExpectedException(
                socketErrorCode,
                "Open conversation failed for user " + context.user->id + ", " + conversationsResultCodeString
            );
        }

        const ConversationOpenedSocketEventPayload conversationOpenedSocketEventPayload(conversationResult.data.value());
        const ConversationOpenedSocketEvent conversationOpendSocketEvent(conversationOpenedSocketEventPayload);
        const std::string serializedConversationOpenedSocketEvent = m_conversationOpenedSocketEventSerializer.Serialize(conversationOpendSocketEvent);

        m_socketServer.SendTo(context.clientSocket, serializedConversationOpenedSocketEvent);
    };
}

// ***********
// * PRIVATE *
// ***********
SocketErrorCode ConversationsHandler::ConvertConversationsResultCodeToSocketErrorCode(ConversationsService::ConversationsResultCode conversationsResultCode)
{
    switch (conversationsResultCode)
    {
        case ConversationsService::ConversationsResultCode::USERS_EMPTY:
        case ConversationsService::ConversationsResultCode::TOO_MANY_USERS:
            return SocketErrorCode::INVALID_PAYLOAD;

        case ConversationsService::ConversationsResultCode::UNKNOWN_USER:
        case ConversationsService::ConversationsResultCode::UNKNOWN_CONVERSATION:
            return SocketErrorCode::NOT_FOUND;

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return SocketErrorCode::INTERNAL;
    }
}
