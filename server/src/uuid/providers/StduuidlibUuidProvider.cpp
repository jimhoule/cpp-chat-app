#include "uuid/providers/StduuidlibUuidProvider.h"

#include <uuid.h>

// **********
// * PUBLIC *
// **********
StduuidlibUuidProvider::StduuidlibUuidProvider(Logger& logger)
    : m_logger(logger)
{}

/**
 * NOTES:
 *  - random_device is handed straight to the generator instead of seeding mt19937, because these uuids become session tokens and mt19937 state is recoverable from its 
 *    own output
 *  - static thread_local because opening the system entropy source on every call is wasteful and random_device is not guaranteed thread safe
 */
std::string StduuidlibUuidProvider::Generate() const
{
      static thread_local std::random_device randomDevice = {};
      static thread_local uuids::basic_uuid_random_generator<std::random_device> GenerateUuid(randomDevice);

      return uuids::to_string(GenerateUuid());
}
