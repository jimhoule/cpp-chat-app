#include "serializer/MessageCreatedSocketEventSerializer.h"

#include <nlohmann/json.hpp>

// **********
// * PUBLIC *
// **********
std::string MessageCreatedSocketEventSerializer::Serialize(const MessageCreatedSocketEvent& messageCreatedSocketEvent)
{
    nlohmann::json messageJson = {};
    messageJson["id"] = messageCreatedSocketEvent.payload.message.id;
    messageJson["conversationId"] = messageCreatedSocketEvent.payload.message.conversationId;
    messageJson["senderId"] = messageCreatedSocketEvent.payload.message.senderId;
    messageJson["text"] = messageCreatedSocketEvent.payload.message.text;
    messageJson["createdAt"] = messageCreatedSocketEvent.payload.message.createdAt;

    nlohmann::json payloadJson = {};
    payloadJson["message"] = messageJson;

    nlohmann::json json = {};
    json["name"] = messageCreatedSocketEvent.name;
    json["payload"] = payloadJson;

    return json.dump();
}
