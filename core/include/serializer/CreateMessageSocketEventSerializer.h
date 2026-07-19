#pragma once

#include "ISerializer.h"
#include "socket/CreateMessageSocketEvent.h"

class CreateMessageSocketEventSerializer: public ISerializer<CreateMessageSocketEvent, std::string>
{
public:
    CreateMessageSocketEventSerializer() = default;

    std::string Serialize(const CreateMessageSocketEvent& createMessageSocketEvent) override;
};
