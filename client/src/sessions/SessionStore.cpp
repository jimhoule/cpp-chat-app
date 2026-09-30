#include "sessions/SessionStore.h"

#include "json/Json.h"
#include "json/JsonException.h"
#include "log/Logger.h"

#include <cstdlib>
#include <fstream>
#include <sstream>

// **********
// * PUBLIC *
// **********
SessionStore::SessionStore(Logger& logger)
    : m_logger(logger)
    , m_filePath(ResolveDirectoryPath() / "session")
{}

void SessionStore::Clear()
{
    /**
     * NOTES:
     *  - Error code overload does not throw and a file that is already gone is not a failure
     *  - errorCode initialized to {} will have a value of 0 (which means no error)
     */
    std::error_code errorCode = {};
    std::filesystem::remove(m_filePath, errorCode);
    if (errorCode)
    {
        m_logger.Warning("Could not clear session, " + errorCode.message());
    }
}

std::optional<std::string> SessionStore::Load() const
{
    std::ifstream sessionFile(m_filePath);
    if (!sessionFile.is_open())
    {
        return std::nullopt;
    }

    std::stringstream jsonStringStream = {};
    jsonStringStream << sessionFile.rdbuf();

    try
    {
        const Json json = Json::Parse(jsonStringStream.str());
        const std::string sessionId = json.GetString("sessionId");
        // NOTE: An empty session id means no session
        if (sessionId.empty())
        {
            return std::nullopt;
        }

        return sessionId;
    }
    catch (const JsonException& jsonException)
    {
        // NOTE: A corrupted file is the same as no session, reading it must never stop the app from starting
        m_logger.Warning("Could not read session, " + std::string(jsonException.what()));
        return std::nullopt;
    }
}

void SessionStore::Save(const std::string& sessionId)
{
    std::error_code errorCode = {};

    // NOTE: Directory does not exist on a first run
    std::filesystem::create_directories(m_filePath.parent_path(), errorCode);
    if (errorCode)
    {
        m_logger.Error("Could not create session directory, " + errorCode.message());
        return;
    }

    std::ofstream sessionFile(m_filePath, std::ios::trunc);
    if (!sessionFile.is_open())
    {
        m_logger.Error("could not open session file for writing");
        return;
    }

    Json json = {};
    json.Set("sessionId", sessionId);

    sessionFile << json.ToString();
    // NOTE: Closed before the permission are set so nothing is still buffered
    sessionFile.close();

    // NOTE: Only the owner can read the session id, the default would let every account on the machine read it
    std::filesystem::permissions(
        m_filePath,
        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write,
        std::filesystem::perm_options::replace,
        errorCode
    );
    if (errorCode)
    {
        m_logger.Warning("Could not restrict session file permissions, " + errorCode.message());
    }
}

// ***********
// * PRIVATE *
// ***********
std::filesystem::path SessionStore::ResolveDirectoryPath() const
{
    // NOTE: Allows pointing at a throwaway directory without rebuilding
    const char* dataDirectory = std::getenv("LOCAL_DATA_DIRECTORY");
    const std::filesystem::path dataDirectoryPath = dataDirectory != nullptr ? dataDirectory : "";
    // Guards against CHAT_DATA_DIR being an empty string
    if (!dataDirectoryPath.empty())
    {
        return dataDirectoryPath;
    }

    const char* home = std::getenv(APP_DATA_HOME_VARIABLE);
    const std::filesystem::path homePath = home != nullptr ? home : "";
    // Uses working directory to keep the app useable if HOME is not set
    if (homePath.empty())
    {
        m_logger.Warning("HOME is not set, storing session in the working directory");
        return std::filesystem::path(".");
    }

    return homePath / APP_DATA_DIRECTORY / APP_NAME / APP_BUILD_TYPE;
}
