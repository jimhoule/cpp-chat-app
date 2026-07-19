#pragma once

#include "messages/repositories/IMessagesRepository.h"

struct CreateMessageDto
{
    std::string conversationId;
    std::string senderId;
    std::string text;
};

class MessagesService
{
public:
    MessagesService(std::unique_ptr<IMessagesRepository> messagesRepository);

    Message Create(const CreateMessageDto& createMessageDto);

private:
    std::unique_ptr<IMessagesRepository> m_messagesRepository = nullptr;
};
