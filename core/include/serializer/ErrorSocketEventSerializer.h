#pragma once

#include "ISerializer.h"
#include "socket/ErrorSocketEvent.h"

class ErrorSocketEventSerializer: public ISerializer<ErrorSocketEvent, std::string>
{
public:
    ErrorSocketEventSerializer() = default;

    std::string Serialize(const ErrorSocketEvent& errorSocketEvent) override;
};
