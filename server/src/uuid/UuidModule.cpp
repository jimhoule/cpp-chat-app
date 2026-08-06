#include "uuid/UuidModule.h"

#include "uuid/providers/StduuidlibUuidProvider.h"

// **********
// * PUBLIC *
// **********
UuidModule::UuidModule()
    : m_uuidProviderLogger("UUID", "server/src/uuid/providers/StduuidlibUuidProvider")
    , m_uuidServiceLogger("UUID", "server/src/uuid/UuidService")
    , m_uuidService(std::make_unique<StduuidlibUuidProvider>(m_uuidProviderLogger), m_uuidServiceLogger)
{}

UuidService& UuidModule::GetService()
{
    return m_uuidService;
}
