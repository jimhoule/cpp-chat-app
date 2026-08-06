#pragma once

#include "uuid/providers/IUuidProvider.h"

#include <string>

class Logger;

class StduuidlibUuidProvider : public IUuidProvider
{
public:
    StduuidlibUuidProvider(Logger& logger);

    std::string Generate() const override;

private:
    // NOTE: Borrowed from the module, which owns it and outlives this provider
    Logger& m_logger;
};
