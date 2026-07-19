#include "serializer/CreateMessageSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string CreateMessageSocketEventSerializer::Serialize(const CreateMessageSocketEvent& createMessageSocketEvent)
{
    nlohmann::json payloadJson;
    payloadJson["conversationID"] = createMessageSocketEvent.Payload.conversationID;
    payloadJson["text"] = createMessageSocketEvent.Payload.text;

    nlohmann::json json;
    json["name"] = createMessageSocketEvent.Name;
    json["payload"] = payloadJson;

    return json.dump();
}
