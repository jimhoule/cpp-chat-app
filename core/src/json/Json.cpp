#include "json/Json.h"

// **********
// * PUBLIC *
// **********
bool Json::GetBool(const std::string& key) const
{
    const nlohmann::json& fieldJson = GetField(key);
    if (!fieldJson.is_boolean())
    {
        throw JsonException("Field '" + key + "' is not a boolean");
    }

    return fieldJson.get<bool>();
}

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

std::vector<Json> Json::GetObjectArray(const std::string& key) const
{
    const nlohmann::json& fieldJson = GetField(key);
    if (!fieldJson.is_array())
    {
        throw JsonException("Field '" + key + "' is not an array");
    }

    std::vector<Json> jsons = {};
    jsons.reserve(fieldJson.size());

    for (const nlohmann::json& valueJson : fieldJson)
    {
        if (!valueJson.is_object())
        {
            throw JsonException("Field '" + key + "' has a value that is not an object");
        }

        jsons.push_back(Json(valueJson));
    }

    return jsons;
}

std::vector<std::string> Json::GetStringArray(const std::string& key) const
{
    const nlohmann::json& fieldJson = GetField(key);
    if (!fieldJson.is_array())
    {
        throw JsonException("Field '" + key + "' is not an array");
    }

    std::vector<std::string> strings = {};
    for (const nlohmann::json& valueJson : fieldJson)
    {
        // NOTE: Checked per value, the array comes from the other side of the socket and can be mixed
        if (!valueJson.is_string())
        {
            throw JsonException("Field '" + key + "' contains a value that is not a string");
        }

        strings.push_back(valueJson.get<std::string>());
    }

    return strings;
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

bool Json::IsNull(const std::string& key) const
{
    const nlohmann::json& fieldJson = GetField(key);
    return fieldJson.is_null();
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

void Json::Set(const std::string& key, bool boolValue)
{
    m_json[key] = boolValue;
}

void Json::Set(const std::string& key, const char* cStringValue)
{
    // NOTE: Forwards to the std::string overload, the header explains why this one has to exist
    Set(key, std::string(cStringValue));
}

void Json::Set(const std::string& key, const Json& jsonValue)
{
    // NOTE: m_json is reachable here because private access is per class, not per object
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

void Json::Set(const std::string& key, const std::vector<Json>& jsonValues)
{
    // NOTE: Starts from an empty array so an empty vector writes [] instead of null
    nlohmann::json arrayJson = nlohmann::json::array();
    for (const Json& jsonValue : jsonValues)
    {
        // NOTE: m_json is reachable here because private access is per class, not per object
        arrayJson.push_back(jsonValue.m_json);
    }

    m_json[key] = std::move(arrayJson);
}

void Json::Set(const std::string& key, const std::vector<std::string>& stringValues)
{
    // NOTE: nlohmann converts a vector of strings to a json array, an empty vector writes []
    m_json[key] = stringValues;
}

void Json::SetNull(const std::string& key)
{
    m_json[key] = nullptr;
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