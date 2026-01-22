#pragma once

#include <string>

template<typename TData, typename TDeserialized>
class IDeserializer
{
public:
    virtual ~IDeserializer() = default;

    virtual TDeserialized Deserialize(const TData& Data) = 0;
};
