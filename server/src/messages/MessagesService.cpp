#include "messages/MessagesService.h"

#include "log/Logger.h"
#include "uuid/UuidService.h"

constexpr size_t MAX_MESSAGE_TEXT_LENGTH = 4000;

std::string ConvertMessagesResultCodeToString(MessagesResultCode messagesResultCode)
{
    switch (messagesResultCode)
    {
        case MessagesResultCode::OK:
            return "OK";

        case MessagesResultCode::TEXT_EMPTY:
            return "TEXT EMPTY";

        case MessagesResultCode::TEXT_TOO_LONG:
            return "TEXT TOO LONG";

        // NOTE: Handles cases where the enum value might be out of range
        default:
            return "Unknown messages result code";
    }
}

// **********
// * PUBLIC *
// **********
MessagesService::MessagesService(std::unique_ptr<IMessagesRepository> messagesRepository, UuidService& uuidService, Logger& logger)
    : m_messagesRepository(std::move(messagesRepository))
    , m_uuidService(uuidService)
    , m_logger(logger)
{}

MessageResult MessagesService::Create(const CreateMessageDto& createMessageDto)
{
    MessageResult messageResult = {};

    // Validates text
    if (createMessageDto.text.empty())
    {
        messageResult.code = MessagesResultCode::TEXT_EMPTY;
        return messageResult;
    }

    if (createMessageDto.text.size() > MAX_MESSAGE_TEXT_LENGTH)
    {
        messageResult.code = MessagesResultCode::TEXT_TOO_LONG;
        return messageResult;
    }

    // Creates message
    Message message = {};
    message.id = m_uuidService.Generate();
    message.conversationId = createMessageDto.conversationId;
    message.senderId = createMessageDto.senderId;
    message.text = createMessageDto.text;
    message.createdAt = std::time(0);

    messageResult.data = m_messagesRepository->Create(message);

    // NOTE: Logs the text length, never the text itself
    m_logger.Info("Created message " + message.id + " in conversation " + message.conversationId + " from user " + message.senderId + " (" + std::to_string(message.text.size()) + " chars)");

    return messageResult;
}
