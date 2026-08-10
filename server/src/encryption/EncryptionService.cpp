#include "encryption/EncryptionService.h"

// **********
// * PUBLIC *
// **********
EncryptionService::EncryptionService(std::unique_ptr<IEncryptionProvider> encryptionProvider, Logger& logger)
    : m_encryptionProvider(std::move(encryptionProvider))
    , m_logger(logger)
{}

std::string EncryptionService::Encrypt(const EncryptStringDto& encryptStringDto) const
{
    return m_encryptionProvider->Encrypt(encryptStringDto.string);
}

bool EncryptionService::Verify(const VerifyStringDto& verifyStringDto) const
{
    return m_encryptionProvider->Verify(verifyStringDto.string, verifyStringDto.hashedString);
}
