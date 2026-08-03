#pragma once

#include <stdexcept>
#include <string>

class JsonException : public std::runtime_error
{
public:
    JsonException(const std::string& message);
};
