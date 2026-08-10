#pragma once

#include "log/Logger.h"
#include "encryption/EncryptionService.h"

class EncryptionModule
{
public:
    EncryptionModule();

    // Deletes move constructor and assign operator
    EncryptionModule(const EncryptionModule&) = delete;
    EncryptionModule& operator=(const EncryptionModule&) = delete;

    EncryptionService& GetService();

private:
    Logger m_encryptionProviderLogger;
    Logger m_encryptionServiceLogger;

    EncryptionService m_encryptionService;
};