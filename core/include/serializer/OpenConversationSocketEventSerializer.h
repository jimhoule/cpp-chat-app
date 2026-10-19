#pragma once

#include "serializer/ISerializer.h"
#include "socket/OpenConversationSocketEvent.h"

class OpenConversationSocketEventSerializer: public ISerializer<OpenConversationSocketEvent, std::string>
{
public:
    OpenConversationSocketEventSerializer() = default;

    std::string Serialize(const OpenConversationSocketEvent& openConversationSocketEvent) override;
};
