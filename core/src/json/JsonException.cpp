#include "json/JsonException.h"

// **********
// * PUBLIC *
// **********
JsonException::JsonException(const std::string& message)
    : std::runtime_error(message)
{}