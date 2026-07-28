#pragma once

#include <string>

template<typename TData, typename TDeserialized>
class IDeserializer
{
public:
    IDeserializer() = default;
    virtual ~IDeserializer() = default;

    // Rule of Five: Explicitly delete copy/move to prevent slicing
    IDeserializer(const IDeserializer&) = delete;
    IDeserializer& operator=(const IDeserializer&) = delete;
    IDeserializer(IDeserializer&&) = delete;
    IDeserializer& operator=(IDeserializer&&) = delete;

    virtual TDeserialized Deserialize(const TData& data) = 0;
};
