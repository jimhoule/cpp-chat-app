#pragma once

#include "log/Logger.h"
#include "uuid/UuidService.h"

class UuidModule
{
public:
    UuidModule();

    // Deletes move constructor and assign operator
    UuidModule(const UuidModule&) = delete;
    UuidModule& operator=(const UuidModule&) = delete;

    UuidService& GetService();

private:
    Logger m_uuidProviderLogger;
    Logger m_uuidServiceLogger;

    UuidService m_uuidService;
};