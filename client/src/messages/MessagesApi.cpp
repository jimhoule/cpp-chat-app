#include "messages/MessagesApi.h"

// **********
// * PUBLIC *
// **********
MessagesApi::MessagesApi(SocketClient& socketClient, Logger& logger)
    : m_socketClient(socketClient)
    , m_logger(logger)
{
    m_socketClient.On(SocketEventName::ALL_OPEN_MESSAGES_READ_STATES_FOUND_BY_USER_ID, this, &MessagesApi::HandleAllOpenMessagesReadStatesFound);
    m_socketClient.On(SocketEventName::MESSAGE_CREATED, this, &MessagesApi::HandleMessageCreated);
    m_socketClient.On(SocketEventName::MESSAGES_MARKED_READ, this, &MessagesApi::HandleMessagesMarkedRead);
    m_socketClient.On(SocketEventName::MESSAGES_PAGE_FOUND_BY_CONVERSATION_ID, this, &MessagesApi::HandleMessagesPageFound);
    m_socketClient.On(SocketEventName::MESSAGES_READ_STATE_FOUND_BY_CONVERSATION_ID, this, &MessagesApi::HandleMessagesReadStateFound);
}

MessagesApi::~MessagesApi()
{
    m_socketClient.Off(SocketEventName::ALL_OPEN_MESSAGES_READ_STATES_FOUND_BY_USER_ID, this);
    m_socketClient.Off(SocketEventName::MESSAGE_CREATED, this);
    m_socketClient.Off(SocketEventName::MESSAGES_MARKED_READ, this);
    m_socketClient.Off(SocketEventName::MESSAGES_PAGE_FOUND_BY_CONVERSATION_ID, this);
    m_socketClient.Off(SocketEventName::MESSAGES_READ_STATE_FOUND_BY_CONVERSATION_ID, this);
}

IObservable<AllOpenMessagesReadStatesFoundEvent>& MessagesApi::GetAllOpenMessagesReadStatesFoundSubject()
{
    return m_allOpenMessagesReadStatesFoundSubject;
}

IObservable<MessageCreatedEvent>& MessagesApi::GetMessageCreatedSubject()
{
    return m_messageCreatedSubject;
}

IObservable<MessagesMarkedReadEvent>& MessagesApi::GetMessagesMarkedReadSubject()
{
    return m_messagesMarkedReadSubject;
}

IObservable<MessagesPageFoundEvent>& MessagesApi::GetMessagesPageFoundSubject()
{
    return m_messagesPageFoundSubject;
}

IObservable<MessagesReadStateFoundEvent>& MessagesApi::GetMessagesReadStateFoundSubject()
{
    return m_messagesReadStateFoundSubject;
}

void MessagesApi::Create(const CreateMessageParams& createMessageParams)
{
    // Serializes create message socket event
    const CreateMessageSocketEventPayload createMessageSocketEventPayload(
        createMessageParams.conversationId,
        createMessageParams.text
    );
    const CreateMessageSocketEvent createMessageSocketEvent(createMessageSocketEventPayload);
    const std::string serializedCreateMessageSocketEvent = m_createMessageSocketEventSerializer.Serialize(createMessageSocketEvent);

    // Sends create message socket event
    m_socketClient.Send(serializedCreateMessageSocketEvent);
}

void MessagesApi::FindAllOpenReadStates()
{
    // Serializes find all open messages read states by user id socket event
    const FindAllOpenMessagesReadStatesByUserIdSocketEventPayload findAllOpenMessagesReadStatesByUserIdSocketEventPayload;
    const FindAllOpenMessagesReadStatesByUserIdSocketEvent findAllOpenMessagesReadStatesByUserIdSocketEvent(findAllOpenMessagesReadStatesByUserIdSocketEventPayload);
    const std::string serializedFindAllOpenMessagesReadStatesByUserIdSocketEvent = m_findAllOpenMessagesReadStatesByUserIdSocketEventSerializer.Serialize(findAllOpenMessagesReadStatesByUserIdSocketEvent);

    // Sends find all open messages read states by user id socket event
    m_socketClient.Send(serializedFindAllOpenMessagesReadStatesByUserIdSocketEvent);
}

void MessagesApi::FindPage(const FindMessagesPageParams& findMessagesPageParams)
{
    // Serializes find messages page by conversation id socket event
    const FindMessagesPageByConversationIdSocketEventPayload findMessagesPageByConversationIdSocketEventPayload(
        findMessagesPageParams.conversationId,
        findMessagesPageParams.cursor
    );
    const FindMessagesPageByConversationIdSocketEvent findMessagesPageByConversationIdSocketEvent(findMessagesPageByConversationIdSocketEventPayload);
    const std::string serializedFindMessagesPageByConversationIdSocketEvent = m_findMessagesPageByConversationIdSocketEventSerializer.Serialize(findMessagesPageByConversationIdSocketEvent);

    // Sends find messages page by conversation id socket event
    m_socketClient.Send(serializedFindMessagesPageByConversationIdSocketEvent);
}

void MessagesApi::FindReadState(const FindMessagesReadStateParams& findMessagesReadStateParams)
{
    // Serializes find messages read state by conversation id socket event
    const FindMessagesReadStateByConversationIdSocketEventPayload findMessagesReadStateByConversationIdSocketEventPayload(findMessagesReadStateParams.conversationId);
    const FindMessagesReadStateByConversationIdSocketEvent findMessagesReadStateByConversationIdSocketEvent(findMessagesReadStateByConversationIdSocketEventPayload);
    const std::string serializedFindMessagesReadStateByConversationIdSocketEvent = m_findMessagesReadStateByConversationIdSocketEventSerializer.Serialize(findMessagesReadStateByConversationIdSocketEvent);

    // Sends find messages read state by conversation id socket event
    m_socketClient.Send(serializedFindMessagesReadStateByConversationIdSocketEvent);
}

void MessagesApi::MarkRead(const MarkMessagesReadParams& markMessagesReadParams)
{
    // Serializes mark messages read socket event
    const MarkMessagesReadSocketEventPayload markMessagesReadSocketEventPayload(
        markMessagesReadParams.conversationId,
        markMessagesReadParams.messageId
    );
    const MarkMessagesReadSocketEvent markMessagesReadSocketEvent(markMessagesReadSocketEventPayload);
    const std::string serializedMarkMessagesReadSocketEvent = m_markMessagesReadSocketEventSerializer.Serialize(markMessagesReadSocketEvent);

    // Sends mark messages read socket event
    m_socketClient.Send(serializedMarkMessagesReadSocketEvent);
}

// ***********
// * PRIVATE *
// ***********
void MessagesApi::HandleAllOpenMessagesReadStatesFound(const SocketClient::EventContext& context)
{
    const AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload allOpenMessagesReadStatesFoundByUserIdSocketEventPayload = m_allOpenMessagesReadStatesFoundByUserIdSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

    // Sends event to subcriptions
    const AllOpenMessagesReadStatesFoundEvent allOpenMessagesReadStatesFoundEvent(allOpenMessagesReadStatesFoundByUserIdSocketEventPayload.messagesReadStates);
    m_allOpenMessagesReadStatesFoundSubject.Notify(allOpenMessagesReadStatesFoundEvent);
}

void MessagesApi::HandleMessageCreated(const SocketClient::EventContext& context)
{
    const MessageCreatedSocketEventPayload messageCreatedSocketEventPayload = m_messageCreatedSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

    // Sends event to subcriptions
    const MessageCreatedEvent messageCreatedEvent(messageCreatedSocketEventPayload.message);
    m_messageCreatedSubject.Notify(messageCreatedEvent);
}

void MessagesApi::HandleMessagesMarkedRead(const SocketClient::EventContext& context)
{
    const MessagesMarkedReadSocketEventPayload messagesMarkedReadSocketEventPayload = m_messagesMarkedReadSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

    // Sends event to subcriptions
    const MessagesMarkedReadEvent messagesMarkedReadEvent(messagesMarkedReadSocketEventPayload.messagesReadState);
    m_messagesMarkedReadSubject.Notify(messagesMarkedReadEvent);
}

void MessagesApi::HandleMessagesPageFound(const SocketClient::EventContext& context)
{
    const MessagesPageFoundByConversationIdSocketEventPayload messagesPageFoundByConversationIdSocketEventPayload = m_messagesPageFoundByConversationIdSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

    // Sends event to subcriptions
    const MessagesPageFoundEvent messagesPageFoundEvent(
        messagesPageFoundByConversationIdSocketEventPayload.messagesPage,
        messagesPageFoundByConversationIdSocketEventPayload.conversationId
    );
    m_messagesPageFoundSubject.Notify(messagesPageFoundEvent);
}

void MessagesApi::HandleMessagesReadStateFound(const SocketClient::EventContext& context)
{
    const MessagesReadStateFoundByConversationIdSocketEventPayload messagesReadStateFoundByConversationIdSocketEventPayload = m_messagesReadStateFoundByConversationIdSocketEventPayloadDeserializer.Deserialize(context.serializedPayload);

    // Sends event to subcriptions
    const MessagesReadStateFoundEvent messagesReadStateFoundEvent(messagesReadStateFoundByConversationIdSocketEventPayload.messagesReadState);
    m_messagesReadStateFoundSubject.Notify(messagesReadStateFoundEvent);
}
