#include "messages/MessagesApi.h"

#include <iostream>

// **********
// * PUBLIC *
// **********
MessagesApi::MessagesApi(std::shared_ptr<SocketClient> socketClient) : m_socketClient(socketClient)
{}

void MessagesApi::Create(const CreateMessageParams& createMessageParams)
{
    // Serializes create message socket event
    CreateMessageSocketEventPayload createMessageSocketEventPayload(createMessageParams.conversationID, createMessageParams.text);
    CreateMessageSocketEvent createMessageSocketEvent(createMessageSocketEventPayload);
    std::string serializedCreateMessageSocketEvent = m_createMessageSocketEventSerializer.Serialize(createMessageSocketEvent);

    // Sends create message socket event
    m_socketClient->Send(serializedCreateMessageSocketEvent);
}
