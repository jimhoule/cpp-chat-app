#pragma once

#include "IDeserializer.h"
#include "socket/FindConversationByUserIdsSocketEvent.h"

class FindConversationByUserIdsSocketEventPayloadDeserializer: public IDeserializer<std::string, FindConversationByUserIdsSocketEventPayload>
{
public:
    FindConversationByUserIdsSocketEventPayloadDeserializer() = default;

    FindConversationByUserIdsSocketEventPayload Deserialize(const std::string& jsonString) override;
};
