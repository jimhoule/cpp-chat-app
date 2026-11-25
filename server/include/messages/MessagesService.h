#pragma once

#include "models/Message.h"
#include "messages/repositories/IMessagesRepository.h"
#include "results/ServiceResult.h"
#include "views/ConversationView.h"
#include "views/MessagesPageView.h"
#include "views/MessagesReadStateView.h"
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
        TEXT_TOO_LONG,
        UNKNOWN_MESSAGE
    };

    struct CreateMessageData
    {
        MessageView message;
        ConversationView conversation;
        // NOTE: User who had closed the conversation (The message reopened it for them)
        std::vector<std::string> reopenedUserIds;
    };

    using CreateMessageResult = ServiceResult<MessagesResultCode, std::optional<CreateMessageData>>;
    using MessagesReadStateResult = ServiceResult<MessagesResultCode, std::optional<MessagesReadStateView>>;
    using MessagesReadStatesResult = ServiceResult<MessagesResultCode, std::vector<MessagesReadStateView>>;
    using MessagesPageResult = ServiceResult<MessagesResultCode, MessagesPageView>;

    struct CreateMessageDto
    {
        std::string conversationId;
        std::string senderId;
        std::string text;
    };

    struct FindAllOpenMessagesReadStatesByUserIdDto
    {
        std::string userId;
    };

    struct FindMessagesPageByConversationIdDto
    {
        std::string conversationId;
        std::string userId;
        // NOTE: Empty on the first page (otherwise it is the next cursor of the previous page)
        std::string cursor;
    };

    struct FindMessagesReadStateByConversationIdDto
    {
        std::string conversationId;
        std::string userId;
    };

    struct MarkMessagesReadDto
    {
        std::string conversationId;
        std::string userId;
        // NOTE: The newest message id on the client's screen (every message up to this one is marked read)
        std::string messageId;
    };

    MessagesService(std::unique_ptr<IMessagesRepository> messagesRepository, ConversationsService& conversationsService, UuidService& uuidService, Logger& logger);

    CreateMessageResult Create(const CreateMessageDto& createMessageDto);
    MessagesReadStatesResult FindAllOpenReadStatesByUserId(const FindAllOpenMessagesReadStatesByUserIdDto& findAllOpenMessagesReadStatesByUserIdDto);
    MessagesPageResult FindPageByConversationId(const FindMessagesPageByConversationIdDto& findMessagesPageByConversationIdDto);
    /**
     * NOTES:
     *  - Read state of one conversation
     *  - Used when conversation becomes opened or reopened after login since 
     *    read states are only sent once at login 
     */
    MessagesReadStateResult FindReadStateByConversationId(const FindMessagesReadStateByConversationIdDto& findMessagesReadStateByConversationIdDto);
    MessagesReadStateResult MarkRead(const MarkMessagesReadDto& markMessagesReadDto);
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
