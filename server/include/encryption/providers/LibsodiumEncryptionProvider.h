#pragma once

#include "encryption/providers/IEncryptionProvider.h"

#include <string>

// Forward declarations
class Logger;

class LibsodiumEncryptionProvider : public IEncryptionProvider
{
public:
    LibsodiumEncryptionProvider(Logger& logger);

    std::string Encrypt(const std::string& string) const override;
    bool Verify(const std::string& string, const std::string& hashedString) const override;

private:
    // NOTE: Borrowed from the module, which owns it and outlives this provider
    Logger& m_logger;
};