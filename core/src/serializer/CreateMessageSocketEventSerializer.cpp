#include "serializer/CreateMessageSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string CreateMessageSocketEventSerializer::Serialize(const CreateMessageSocketEvent& createMessageSocketEvent)
{
    nlohmann::json payloadJson;
    payloadJson["conversationId"] = createMessageSocketEvent.payload.conversationId;
    payloadJson["text"] = createMessageSocketEvent.payload.text;

    nlohmann::json json;
    json["name"] = createMessageSocketEvent.name;
    json["payload"] = payloadJson;

    return json.dump();
}
