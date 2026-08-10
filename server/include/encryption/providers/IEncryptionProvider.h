#pragma once

#include <string>

// Forward declarations
class Logger;

class IEncryptionProvider
{
public:
    // NOTE: Explicitly default the constructor (Optional but recommended for clarity)
    IEncryptionProvider() = default;

    // NOTE: ALWAYS provide a public virtual destructor
    virtual ~IEncryptionProvider() = default;

    // Disables copying and moving to prevent slicing
    IEncryptionProvider(const IEncryptionProvider&) = delete;
    IEncryptionProvider& operator=(const IEncryptionProvider&) = delete;

    virtual std::string Encrypt(const std::string& string) const = 0;
    virtual bool Verify(const std::string& string, const std::string& hashedString) const = 0;
};
