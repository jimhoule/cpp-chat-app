#pragma once

#include "ISerializer.h"
#include "socket/CloseConversationSocketEvent.h"

class CloseConversationSocketEventSerializer: public ISerializer<CloseConversationSocketEvent, std::string>
{
public:
    CloseConversationSocketEventSerializer() = default;

    std::string Serialize(const CloseConversationSocketEvent& closeConversationSocketEvent) override;
};
