#pragma once

#include "IDeserializer.h"
#include "socket/SearchUsersSocketEvent.h"

class SearchUsersSocketEventPayloadDeserializer: public IDeserializer<std::string, SearchUsersSocketEventPayload>
{
public:
    SearchUsersSocketEventPayloadDeserializer() = default;

    SearchUsersSocketEventPayload Deserialize(const std::string& jsonString) override;
};
