#include "conversations/ConversationsHandler.h"

#include "exceptions/ExpectedException.h"
#include "socket/SocketServer.h"

SocketErrorCode ConvertConversationsResultCodeToSocketErrorCode(ConversationsResultCode conversationsResultCode)
{
    switch (conversationsResultCode)
    {
        case ConversationsResultCode::USERS_EMPTY:
        case ConversationsResultCode::TOO_MANY_USERS:
            return SocketErrorCode::INVALID_PAYLOAD;

        case ConversationsResultCode::UNKNOWN_USER:
        case ConversationsResultCode::UNKNOWN_CONVERSATION:
            return SocketErrorCode::NOT_FOUND;

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return SocketErrorCode::INTERNAL;
    }
}

// **********
// * PUBLIC *
// **********
ConversationsHandler::ConversationsHandler(SocketServer& socketServer, ConversationsService& conversationsService, Logger& logger)
    : m_socketServer(socketServer)
    , m_conversationsService(conversationsService)
    , m_logger(logger)
{}

SocketServerEventHandler ConversationsHandler::GetFindAllOpenConversationsByUserIdHandler()
{
    return [this](const SocketEventContext& context) {
        // NOTE: The user id comes from the authenticated context, never from the payload
        FindAllOpenConversationsByUserIdDto findAllOpenConversationsByUserIdDto = {};
        findAllOpenConversationsByUserIdDto.userId = context.user->id;
        const ConversationsResult conversationsResult = m_conversationsService.FindAllOpenByUserId(findAllOpenConversationsByUserIdDto);
        if (conversationsResult.code != ConversationsResultCode::OK)
        {
            throw ExpectedException(
                ConvertConversationsResultCodeToSocketErrorCode(conversationsResult.code),
                "Find all open conversations failed for user " + context.user->id + ", " + ConvertConversationsResultCodeToString(conversationsResult.code)
            );
        }

        const AllOpenConversationsFoundByUserIdSocketEventPayload allOpenConversationsFoundByUserIdSocketEventPayload(conversationsResult.data);
        const AllOpenConversationsFoundByUserIdSocketEvent allOpenConversationsFoundByUserIdSocketEvent(allOpenConversationsFoundByUserIdSocketEventPayload);
        const std::string serializedOpenConversationsFoundSocketEvent = m_allOpenConversationsFoundByUserIdSocketEventSerializer.Serialize(allOpenConversationsFoundByUserIdSocketEvent);

        m_socketServer.SendTo(context.clientSocket, serializedOpenConversationsFoundSocketEvent);
    };
}

SocketServerEventHandler ConversationsHandler::GetFindConversationByUserIdsHandler()
{
    return [this](const SocketEventContext& context) {
        const FindConversationByUserIdsSocketEventPayload findConversationByUserIdsSocketEventPayload = m_findConversationByUserIdsSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        FindConversationByUserIdsDto findConversationByUserIdsDto = {};
        // NOTE: The initiator comes from the authenticated context, never from the payload
        findConversationByUserIdsDto.initiatorUserId = context.user->id;
        findConversationByUserIdsDto.userIds = findConversationByUserIdsSocketEventPayload.userIds;
        const ConversationResult conversationResult = m_conversationsService.FindByUserIds(findConversationByUserIdsDto);
        if (conversationResult.code != ConversationsResultCode::OK)
        {
            throw ExpectedException(
                ConvertConversationsResultCodeToSocketErrorCode(conversationResult.code),
                "Find conversation by user ids failed for user " + context.user->id + ", " + ConvertConversationsResultCodeToString(conversationResult.code)
            );
        }

        // NOTE: An empty conversation is a normal answer so optional stays wrapped
        const ConversationFoundByUserIdsSocketEventPayload conversationFoundByUserIdsSocketEventPayload(conversationResult.data);
        const ConversationFoundByUserIdsSocketEvent conversationFoundByUserIdsSocketEvent(conversationFoundByUserIdsSocketEventPayload);
        const std::string serializedConversationFoundByUserIdsSocketEvent = m_conversationFoundByUserIdsSocketEventSerializer.Serialize(conversationFoundByUserIdsSocketEvent);

        m_socketServer.SendTo(context.clientSocket, serializedConversationFoundByUserIdsSocketEvent);
    };
}

SocketServerEventHandler ConversationsHandler::GetCloseConversationHandler()
{
    return [this](const SocketEventContext& context) {
        const CloseConversationSocketEventPayload closeConversationSocketEventPayload = m_closeConversationSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        CloseConversationDto closeConversationDto = {};
        // NOTE: User id comes from the authenticated context, never from the payload
        closeConversationDto.userId = context.user->id;
        closeConversationDto.conversationId = closeConversationSocketEventPayload.conversationId;
        const ConversationResult conversationResult = m_conversationsService.Close(closeConversationDto);
        if (conversationResult.code != ConversationsResultCode::OK)
        {
            throw ExpectedException(
                ConvertConversationsResultCodeToSocketErrorCode(conversationResult.code),
                "Close conversation for user " + context.user->id + ", " + ConvertConversationsResultCodeToString(conversationResult.code)
            );
        }

        const ConversationClosedSocketEventPayload conversationClosedSocketEventPayload(conversationResult.data.value());
        const ConversationClosedSocketEvent conversationClosedSocketEvent(conversationClosedSocketEventPayload);
        const std::string serializedConversationClosedSocketEvent = m_conversationClosedSocketEventSerializer.Serialize(conversationClosedSocketEvent);

        m_socketServer.SendTo(context.clientSocket, serializedConversationClosedSocketEvent);
    };
}

SocketServerEventHandler ConversationsHandler::GetOpenConversationHandler()
{
    return [this](const SocketEventContext& context) {
        const OpenConversationSocketEventPayload openConversationSocketEventPayload = m_openConversationSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

        OpenConversationDto openConversationDto = {};
        // NOTE: The initiator comes from the authenticated context, never from the payload
        openConversationDto.initiatorUserId = context.user->id;
        openConversationDto.userIds = openConversationSocketEventPayload.userIds;
        const ConversationResult conversationResult = m_conversationsService.Open(openConversationDto);
        if (conversationResult.code != ConversationsResultCode::OK)
        {
            throw ExpectedException(
                ConvertConversationsResultCodeToSocketErrorCode(conversationResult.code),
                "Open conversation failed for user " + context.user->id + ", " + ConvertConversationsResultCodeToString(conversationResult.code)
            );
        }

        const ConversationOpenedSocketEventPayload conversationOpenedSocketEventPayload(conversationResult.data.value());
        const ConversationOpenedSocketEvent conversationOpendSocketEvent(conversationOpenedSocketEventPayload);
        const std::string serializedConversationOpenedSocketEvent = m_conversationOpenedSocketEventSerializer.Serialize(conversationOpendSocketEvent);

        m_socketServer.SendTo(context.clientSocket, serializedConversationOpenedSocketEvent);
    };
}
