#include "uuid/UuidService.h"

// **********
// * PUBLIC *
// **********
UuidService::UuidService(std::unique_ptr<IUuidProvider> uuidProvider, Logger& logger)
    : m_uuidProvider(std::move(uuidProvider))
    , m_logger(logger)
{}

std::string UuidService::Generate() const
{
    return m_uuidProvider->Generate();
}
