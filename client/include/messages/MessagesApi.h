#pragma once

#include "serializer/CreateMessageSocketEventSerializer.h"
#include "socket/SocketClient.h"

struct CreateMessageParams
{
    std::string conversationID;
    std::string text;
};

class MessagesApi
{
public:
    MessagesApi(std::shared_ptr<SocketClient> socketClient);

    void Create(const CreateMessageParams& createMessageParams);

private:
    CreateMessageSocketEventSerializer m_createMessageSocketEventSerializer = {};
    std::shared_ptr<SocketClient> m_socketClient = nullptr;
};
