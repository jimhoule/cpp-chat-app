#pragma once

#include "encryption/providers/IEncryptionProvider.h"

#include <memory>
#include <string>

// Forward declarations
class Logger;

class EncryptionService
{
public:
    struct EncryptStringDto
    {
        std::string string;
    };

    struct VerifyStringDto
    {
        std::string string;
        std::string hashedString;

    };

    EncryptionService(std::unique_ptr<IEncryptionProvider> encryptionProvider, Logger& logger);

    std::string Encrypt(const EncryptStringDto& encryptStringDto) const;
    bool Verify(const VerifyStringDto& verifyStringDto) const;

private:
    std::unique_ptr<IEncryptionProvider> m_encryptionProvider = nullptr;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;
};
