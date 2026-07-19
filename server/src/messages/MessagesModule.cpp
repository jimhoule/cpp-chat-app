#include "messages/MessagesModule.h"

#include "messages/repositories/MessagesInMemoryRepository.h"
#include "socket/SocketServer.h"

MessagesModule::MessagesModule(SocketServer& socketServer) : m_messagesService(std::make_unique<MessagesInMemoryRepository>()), m_messagesHandler(socketServer, m_messagesService)
{
    socketServer.On(SocketEventName::CREATE_MESSAGE, m_messagesHandler.GetCreateMessageHandler());
}

MessagesService& MessagesModule::GetService()
{
    return m_messagesService;
}