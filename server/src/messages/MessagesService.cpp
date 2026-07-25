#include "messages/MessagesService.h"

// **********
// * PUBLIC *
// **********
MessagesService::MessagesService(std::unique_ptr<IMessagesRepository> messagesRepository, Logger& logger)
    : m_messagesRepository(std::move(messagesRepository))
    , m_logger(logger)
{}

Message MessagesService::Create(const CreateMessageDto& createMessageDto)
{
    Message message = {};
    message.id = "uuid." + createMessageDto.conversationId + "." + createMessageDto.senderId;
    message.conversationId = createMessageDto.conversationId;
    message.senderId = createMessageDto.senderId;
    message.text = createMessageDto.text;
    message.createdAt = std::time(0);


    return m_messagesRepository->Create(message);
}
