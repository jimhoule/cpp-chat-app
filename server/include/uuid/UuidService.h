#pragma once
  
#include "uuid/providers/IUuidProvider.h"

#include <memory>
#include <string>

// Forward declarations
class Logger;

class UuidService
{
public:
    UuidService(std::unique_ptr<IUuidProvider> uuidProvider, Logger& logger);

    std::string Generate() const;

private:
    std::unique_ptr<IUuidProvider> m_uuidProvider = nullptr;

    // NOTE: Borrowed from the module, which owns it and outlives this service
    Logger& m_logger;
};
