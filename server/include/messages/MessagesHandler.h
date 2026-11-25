#pragma once

#include "deserializer/CreateMessageSocketEventPayloadDeserializer.h"
#include "deserializer/FindMessagesPageByConversationIdSocketEventPayloadDeserializer.h"
#include "deserializer/FindMessagesReadStateByConversationIdSocketEventPayloadDeserializer.h"
#include "deserializer/MarkMessagesReadSocketEventPayloadDeserializer.h"
#include "messages/MessagesService.h"
#include "serializer/AllOpenMessagesReadStatesFoundByUserIdSocketEventSerializer.h"
#include "serializer/ConversationReopenedSocketEventSerializer.h"
#include "serializer/MessageCreatedSocketEventSerializer.h"
#include "serializer/MessagesMarkedReadSocketEventSerializer.h"
#include "serializer/MessagesPageFoundByConversationIdSocketEventSerializer.h"
#include "serializer/MessagesReadStateFoundByConversationIdSocketEventSerializer.h"
#include "socket/SocketErrorCode.h"
#include "socket/SocketServer.h"

// Forward declarations
class Logger;

class MessagesHandler
{
public:
    MessagesHandler(SocketServer& socketServer, MessagesService& messagesService, Logger& logger);

    SocketServer::EventHandler GetFindAllOpenMessagesReadStatesByUserIdHandler();
    SocketServer::EventHandler GetFindMessagesPageByConversationIdHandler();
    SocketServer::EventHandler GetFindMessagesReadStateByConversationIdHandler();
    SocketServer::EventHandler GetCreateMessageHandler();
    SocketServer::EventHandler GetMarkMessagesReadHandler();

private:
    SocketServer& m_socketServer;
    MessagesService& m_messagesService;

    // NOTE: Borrowed from the module, which owns it and outlives this handler
    Logger& m_logger;

    AllOpenMessagesReadStatesFoundByUserIdSocketEventSerializer m_allOpenMessagesReadStatesFoundByUserIdSocketEventSerializer = {};

    ConversationReopenedSocketEventSerializer m_conversationReopenedSocketEventSerializer = {};

    MessageCreatedSocketEventSerializer m_messageCreatedSocketEventSerializer = {};
    CreateMessageSocketEventPayloadDeserializer m_createMessageSocketEventPayloadDeserializer = {};

    MessagesMarkedReadSocketEventSerializer m_messagesMarkedReadSocketEventSerializer = {};
    MarkMessagesReadSocketEventPayloadDeserializer m_markMessagesReadSocketEventPayloadDeserializer = {};

    MessagesPageFoundByConversationIdSocketEventSerializer m_messagesPageFoundByConversationIdSocketEventSerializer = {};
    FindMessagesPageByConversationIdSocketEventPayloadDeserializer m_findMessagesPageByConversationIdSocketEventPayloadDeserializer = {};

    MessagesReadStateFoundByConversationIdSocketEventSerializer m_messagesReadStateFoundByConversationIdSocketEventSerializer = {};
    FindMessagesReadStateByConversationIdSocketEventPayloadDeserializer m_findMessagesReadStateByConversationIdSocketEventPayloadDeserializer = {};

    SocketErrorCode ConvertMessagesResultCodeToSocketErrorCode(MessagesService::MessagesResultCode messagesResultCode);
};
