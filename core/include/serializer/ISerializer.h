#pragma once

#include <string>

template<typename TData, typename TSerialized>
class ISerializer
{
public:
    virtual ~ISerializer() = default;

    virtual TSerialized Serialize(const TData& Data) = 0;
};
