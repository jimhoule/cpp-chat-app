#include "deserializer/MessageCreatedSocketEventPayloadDeserializer.h"

#include "json/Json.h"
#include "json/MessageJson.h"
#include "views/MessageView.h"

#include <utility>

// **********
// * PUBLIC *
// **********
MessageCreatedSocketEventPayload MessageCreatedSocketEventPayloadDeserializer::Deserialize(const std::string& jsonString)
{
    const Json json = Json::Parse(jsonString);
    const Json messageViewJson = json.GetObject("message");

    MessageView messageView = ConvertJsonToMessageView(messageViewJson);

    MessageCreatedSocketEventPayload messageCreatedSocketEventPayload = {};
    messageCreatedSocketEventPayload.message = std::move(messageView);

    return messageCreatedSocketEventPayload;
}
