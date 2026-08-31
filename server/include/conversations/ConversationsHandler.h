#pragma once

#include "conversations/ConversationsService.h"
#include "deserializer/CloseConversationSocketEventPayloadDeserializer.h"
#include "deserializer/FindConversationByUserIdsSocketEventPayloadDeserializer.h"
#include "deserializer/OpenConversationSocketEventPayloadDeserializer.h"
#include "serializer/AllOpenConversationsFoundByUserIdSocketEventSerializer.h"
#include "serializer/ConversationClosedSocketEventSerializer.h"
#include "serializer/ConversationFoundByUserIdsSocketEventSerializer.h"
#include "serializer/ConversationOpenedSocketEventSerializer.h"
#include "socket/SocketErrorCode.h"
#include "socket/SocketServer.h"

// Forward declarations
class Logger;

class ConversationsHandler
{
public:
    ConversationsHandler(SocketServer& socketServer, ConversationsService& conversationsService, Logger& logger);

    SocketServer::EventHandler GetFindAllOpenConversationsByUserIdHandler();
    SocketServer::EventHandler GetFindConversationByUserIdsHandler();
    SocketServer::EventHandler GetCloseConversationHandler();
    SocketServer::EventHandler GetOpenConversationHandler();

private:
    SocketServer& m_socketServer;
    ConversationsService& m_conversationsService;

    // NOTE: Borrowed from the module, which owns it and outlives this handler
    Logger& m_logger;

    AllOpenConversationsFoundByUserIdSocketEventSerializer m_allOpenConversationsFoundByUserIdSocketEventSerializer = {};

    ConversationClosedSocketEventSerializer m_conversationClosedSocketEventSerializer = {};
    CloseConversationSocketEventPayloadDeserializer m_closeConversationSocketEventPayloadDeserializer = {};

    ConversationFoundByUserIdsSocketEventSerializer m_conversationFoundByUserIdsSocketEventSerializer = {};
    FindConversationByUserIdsSocketEventPayloadDeserializer m_findConversationByUserIdsSocketEventPayloadDeserializer = {};

    ConversationOpenedSocketEventSerializer m_conversationOpenedSocketEventSerializer = {};
    OpenConversationSocketEventPayloadDeserializer m_openConversationSocketEventPayloadDeserializer = {};

    SocketErrorCode ConvertConversationsResultCodeToSocketErrorCode(ConversationsService::ConversationsResultCode conversationsResultCode);
};
