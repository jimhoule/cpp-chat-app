#pragma once

#include <string>

template<typename TData, typename TSerialized>
class ISerializer
{
public:
    ISerializer() = default;
    virtual ~ISerializer() = default;

    // Rule of Five: Explicitly delete copy/move to prevent slicing
    ISerializer(const ISerializer&) = delete;
    ISerializer& operator=(const ISerializer&) = delete;
    ISerializer(ISerializer&&) = delete;
    ISerializer& operator=(ISerializer&&) = delete;

    virtual TSerialized Serialize(const TData& Data) = 0;
};
