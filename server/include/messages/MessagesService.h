#pragma once

#include "messages/repositories/IMessagesRepository.h"

// Forward declarations
class Logger;

struct CreateMessageDto
{
    std::string conversationId;
    std::string senderId;
    std::string text;
};

class MessagesService
{
public:
    MessagesService(std::unique_ptr<IMessagesRepository> messagesRepository, Logger& logger);

    Message Create(const CreateMessageDto& createMessageDto);

private:
    std::unique_ptr<IMessagesRepository> m_messagesRepository = nullptr;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;
};
