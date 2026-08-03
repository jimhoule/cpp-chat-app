#include "json/Json.h"

// **********
// * PUBLIC *
// **********
Json Json::GetObject(const std::string& key) const
{
    const nlohmann::json& fieldJson = GetField(key);
    if (!fieldJson.is_object())
    {
        throw JsonException("Field '" + key + "' is not an object");
    }

    return Json(fieldJson);
}

std::string Json::GetRaw(const std::string& key) const
{
    // NOTE: The bytes come from the other side of the socket, replace keeps invalid UTF-8 from throwing
    return GetField(key).dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
}

std::string Json::GetString(const std::string& key) const
{
    const nlohmann::json& fieldJson = GetField(key);
    if (!fieldJson.is_string())
    {
        throw JsonException("Field '" + key + "' is not a string");
    }

    return fieldJson.get<std::string>();
}

std::time_t Json::GetTime(const std::string& key) const
{
    const nlohmann::json& fieldJson = GetField(key);
    if (!fieldJson.is_number_integer())
    {
        throw JsonException("Field '" + key + "' is not a timestamp");
    }

    return fieldJson.get<std::time_t>();
}

Json Json::Parse(const std::string& jsonString)
{
    // NOTE: allow_exceptions = false, a malformed string comes back discarded instead of throwing
    nlohmann::json json = nlohmann::json::parse(jsonString, nullptr, false);
    if (json.is_discarded())
    {
        throw JsonException("String is not valid JSON");
    }

    // NOTE: Every accessor assumes an object, so it is checked once here
    if (!json.is_object())
    {
        throw JsonException("String is not a JSON object");
    }

    return Json(std::move(json));
}

void Json::Set(const std::string& key, const Json& jsonValue)
{
    m_json[key] = jsonValue.m_json;
}

void Json::Set(const std::string& key, const std::string& stringValue)
{
    m_json[key] = stringValue;
}

void Json::Set(const std::string& key, std::time_t timestampValue)
{
    m_json[key] = timestampValue;
}

std::string Json::ToString() const
{
    // NOTE: replace instead of throwing, writing cannot fail so the only json errors are read errors
    return m_json.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
}

// ***********
// * PRIVATE *
// ***********
Json::Json(nlohmann::json json)
    : m_json(std::move(json))
{}

const nlohmann::json& Json::GetField(const std::string& key) const
{
    // NOTE: find, not operator[], which would insert a null field on a non-const json
    nlohmann::json::const_iterator jsonIterator = m_json.find(key);
    if (jsonIterator == m_json.end())
    {
        throw JsonException("Missing field '" + key + "'");
    }

    return *jsonIterator;
}