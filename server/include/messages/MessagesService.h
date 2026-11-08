#pragma once

#include "models/Message.h"
#include "messages/repositories/IMessagesRepository.h"
#include "results/ServiceResult.h"
#include "views/ConversationView.h"
#include "views/MessagesPageView.h"
#include "views/MessageView.h"

#include <optional>
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
    using MessagesPageResult = ServiceResult<MessagesResultCode, MessagesPageView>;

    struct CreateMessageDto
    {
        std::string conversationId;
        std::string senderId;
        std::string text;
    };

    struct FindMessagesPageByConversationIdDto
    {
        std::string conversationId;
        std::string userId;
        // NOTE: Empty on the first page (otherwise it is the next cursor of the previous page)
        std::string cursor;
    };

    MessagesService(std::unique_ptr<IMessagesRepository> messagesRepository, ConversationsService& conversationsService, UuidService& uuidService, Logger& logger);

    CreateMessageResult Create(const CreateMessageDto& createMessageDto);
    MessagesPageResult FindPageByConversationId(const FindMessagesPageByConversationIdDto& findMessagesPageByConversationIdDto);
    std::string ConvertMessagesResultCodeToString(MessagesResultCode messagesResultCode);

private:
    std::unique_ptr<IMessagesRepository> m_messagesRepository = nullptr;

    ConversationsService& m_conversationsService;
    UuidService& m_uuidService;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;

    MessageView ConvertMessageToMessageView(const Message& message) const;
    bool IsConversationUser(const std::string& conversationId, const std::string& userId) const;
};
