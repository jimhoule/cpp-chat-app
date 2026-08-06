#pragma once

#include "messages/repositories/IMessagesRepository.h"
#include "results/ServiceResult.h"

// Forward declarations
class Logger;
class UuidService;

enum class MessagesResultCode
{
    OK,
    TEXT_EMPTY,
    TEXT_TOO_LONG
};

std::string ConvertMessagesResultCodeToString(MessagesResultCode messagesResultCode);

using MessageResult = ServiceResult<MessagesResultCode, std::optional<Message>>;

struct CreateMessageDto
{
    std::string conversationId;
    std::string senderId;
    std::string text;
};

class MessagesService
{
public:
    MessagesService(std::unique_ptr<IMessagesRepository> messagesRepository, UuidService& uuidService, Logger& logger);

    MessageResult Create(const CreateMessageDto& createMessageDto);

private:
    std::unique_ptr<IMessagesRepository> m_messagesRepository = nullptr;

    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;
};
