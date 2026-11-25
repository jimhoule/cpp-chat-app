#pragma once

#include "socket/SocketEvent.h"
#include "views/MessagesReadStateView.h"

struct MessagesReadStateFoundByConversationIdSocketEventPayload
{
    MessagesReadStateFoundByConversationIdSocketEventPayload() = default;
    MessagesReadStateFoundByConversationIdSocketEventPayload(const MessagesReadStateView& messagesReadState)
        : messagesReadState(messagesReadState)
    {}

    MessagesReadStateView messagesReadState;
};

struct MessagesReadStateFoundByConversationIdSocketEvent : public SocketEvent<MessagesReadStateFoundByConversationIdSocketEventPayload>
{
    MessagesReadStateFoundByConversationIdSocketEvent(MessagesReadStateFoundByConversationIdSocketEventPayload payload) :  SocketEvent(SocketEventName::MESSAGES_READ_STATE_FOUND_BY_CONVERSATION_ID, payload)
    {}
};
