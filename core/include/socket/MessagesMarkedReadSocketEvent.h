#pragma once

#include "socket/SocketEvent.h"
#include "views/MessagesReadStateView.h"

struct MessagesMarkedReadSocketEventPayload
{
    MessagesMarkedReadSocketEventPayload() = default;
    MessagesMarkedReadSocketEventPayload(const MessagesReadStateView& messagesReadState)
        : messagesReadState(messagesReadState)
    {}

    MessagesReadStateView messagesReadState;
};

struct MessagesMarkedReadSocketEvent : public SocketEvent<MessagesMarkedReadSocketEventPayload>
{
    MessagesMarkedReadSocketEvent(MessagesMarkedReadSocketEventPayload payload) :  SocketEvent(SocketEventName::MESSAGES_MARKED_READ, payload)
    {}
};
