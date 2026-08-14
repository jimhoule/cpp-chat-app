#pragma once

#include "json/JsonException.h"

#include <ctime>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

class Json
{
public:
    // NOTE: Starts as an empty object, ready to be written to
    Json() = default;

    // NOTE: Throws JsonException if the string is not a valid json object
    static Json Parse(const std::string& jsonString);

    // Reading
    bool GetBool(const std::string& key) const;
    // NOTE: The expected type is stated because the data comes from the other side of the socket
    Json GetObject(const std::string& key) const;
    std::string GetRaw(const std::string& key) const;
    std::string GetString(const std::string& key) const;
    // NOTE: Throws JsonException if the field is not an array or if any of its values is not an object
    std::vector<Json> GetObjectArray(const std::string& key) const;
    // NOTE: Throws JsonException if the field is not an array or if any of its values is not a string
    std::vector<std::string> GetStringArray(const std::string& key) const;
    std::time_t GetTime(const std::string& key) const;
    bool IsNull(const std::string& key) const;

    template<typename TEnum>
    TEnum GetEnum(const std::string& key) const
    {
        const nlohmann::json& fieldJson = GetField(key);
        if (!fieldJson.is_string())
        {
            throw JsonException("Field '" + key + "' is not an enum value");
        }

        // NOTE: Relies on NLOHMANN_JSON_SERIALIZE_ENUM, an unknown value falls back to the first enumerator
        return fieldJson.get<TEnum>();
    }

    // Writing
    void Set(const std::string& key, bool boolValue);
    // NOTE: Without this overload a string literal would pick Set(key, bool), const char* converts to bool as a standard conversion while converting to std::string is user defined and loses
    void Set(const std::string& key, const char* cStringValue);
    // NOTE: The type is known at compile time, so overloads are enough and none of these can fail
    void Set(const std::string& key, const Json& jsonValue);
    void Set(const std::string& key, const std::string& stringValue);
    void Set(const std::string& key, std::time_t timestampValue);
    // NOTE: An empty vector writes an empty array, never null
    void Set(const std::string& key, const std::vector<Json>& jsonValues);
    void Set(const std::string& key, const std::vector<std::string>& stringValues);
    void SetNull(const std::string& key);

    template<typename TEnum>
    void SetEnum(const std::string& key, TEnum enumValue)
    {
        m_json[key] = enumValue;
    }

    std::string ToString() const;

private:
    /**
     * NOTES:
     *  - Private "adopt an existing nlohmann value" constructor
     *  - It exists because two members need to build a Json around a document they already have
     */
    Json(nlohmann::json json);

    // NOTE: Throws JsonException if the field is missing
    const nlohmann::json& GetField(const std::string& key) const;

    nlohmann::json m_json = nlohmann::json::object();
};