#include "encryption/EncryptionModule.h"

#include "encryption/providers/LibsodiumEncryptionProvider.h"

// **********
// * PUBLIC *
// **********
EncryptionModule::EncryptionModule()
    : m_encryptionProviderLogger("ENCRYPTION", "server/encryption/providers/LibsodiumEncryptionProvider")
    , m_encryptionServiceLogger("ENCRYPTION", "server/encryption/EncryptionService")
    , m_encryptionService(std::make_unique<LibsodiumEncryptionProvider>(m_encryptionProviderLogger), m_encryptionServiceLogger)
{}

EncryptionService& EncryptionModule::GetService()
{
    return m_encryptionService;
}
