#pragma once

#include <string>

class IUuidProvider
{
public:
    // NOTE: Explicitly default the constructor (Optional but recommended for clarity)
    IUuidProvider() = default;

    // NOTE: ALWAYS provide a public virtual destructor
    virtual ~IUuidProvider() = default;

    // Disables copying and moving to prevent slicing
    IUuidProvider(const IUuidProvider&) = delete;
    IUuidProvider& operator=(const IUuidProvider&) = delete;

    virtual std::string Generate() const = 0;

};
