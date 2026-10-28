#pragma once

#include "models/Message.h"
#include "messages/repositories/IMessagesRepository.h"
#include "results/ServiceResult.h"
#include "views/ConversationView.h"
#include "views/MessageView.h"

#include <string>
#include <vector>

// Forward declarations
class ConversationsService;
class Logger;
class UuidService;

class MessagesService
{
public:
    enum class MessagesResultCode
    {
        OK,
        NOT_CONVERSATION_USER,
        TEXT_EMPTY,
        TEXT_TOO_LONG
    };

    struct CreateMessageData
    {
        MessageView message;
        ConversationView conversation;
        // NOTE: User who had closed the conversation (The message reopened it for them)
        std::vector<std::string> reopenedUserIds;
    };

    using CreateMessageResult = ServiceResult<MessagesResultCode, std::optional<CreateMessageData>>;

    struct CreateMessageDto
    {
        std::string conversationId;
        std::string senderId;
        std::string text;
    };

    MessagesService(std::unique_ptr<IMessagesRepository> messagesRepository, ConversationsService& conversationsService, UuidService& uuidService, Logger& logger);

    CreateMessageResult Create(const CreateMessageDto& createMessageDto);
    std::string ConvertMessagesResultCodeToString(MessagesResultCode messagesResultCode);

private:
    std::unique_ptr<IMessagesRepository> m_messagesRepository = nullptr;

    ConversationsService& m_conversationsService;
    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;

    MessageView ConvertMessageToMessageView(const Message& message) const;
};
