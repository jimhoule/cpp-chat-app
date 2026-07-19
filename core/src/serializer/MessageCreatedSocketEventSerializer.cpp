#include "serializer/MessageCreatedSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string MessageCreatedSocketEventSerializer::Serialize(const MessageCreatedSocketEvent& messageCreatedSocketEvent)
{
    nlohmann::json messageJson = {};
    messageJson["id"] = messageCreatedSocketEvent.Payload.message.id;
    messageJson["conversationId"] = messageCreatedSocketEvent.Payload.message.conversationId;
    messageJson["senderId"] = messageCreatedSocketEvent.Payload.message.senderId;
    messageJson["text"] = messageCreatedSocketEvent.Payload.message.text;
    messageJson["createdAt"] = messageCreatedSocketEvent.Payload.message.createdAt;

    nlohmann::json payloadJson = {};
    payloadJson["message"] = messageJson;

    nlohmann::json json = {};
    json["name"] = messageCreatedSocketEvent.Name;
    json["payload"] = payloadJson;

    return json.dump();
}
