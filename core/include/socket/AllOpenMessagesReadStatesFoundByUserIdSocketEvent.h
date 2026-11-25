#pragma once

#include "socket/SocketEvent.h"
#include "views/MessagesReadStateView.h"

#include <vector>

struct AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload
{
    AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload() = default;
    AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload(const std::vector<MessagesReadStateView>& messagesReadStates)
        : messagesReadStates(messagesReadStates)
    {}

    std::vector<MessagesReadStateView> messagesReadStates;
};

struct AllOpenMessagesReadStatesFoundByUserIdSocketEvent : public SocketEvent<AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload>
{
    AllOpenMessagesReadStatesFoundByUserIdSocketEvent(AllOpenMessagesReadStatesFoundByUserIdSocketEventPayload payload) :  SocketEvent(SocketEventName::ALL_OPEN_MESSAGES_READ_STATES_FOUND_BY_USER_ID, payload)
    {}
};
