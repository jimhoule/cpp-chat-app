#pragma once

#include <filesystem>
#include <optional>
#include <string>

// Forward declarations
class Logger;

class SessionStore
{
public:
    SessionStore(Logger& logger);

    void Clear();
    std::optional<std::string> Load() const;
    void Save(const std::string& sessionId);

private:
    // NOTE: Declared first so it is bound before the path that resolves using it
    Logger& m_logger;

    std::filesystem::path m_filePath = {};

    std::filesystem::path ResolveDirectoryPath() const;
};
