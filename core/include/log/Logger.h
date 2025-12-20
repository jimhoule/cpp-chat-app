#pragma once

#include "LogLevel.h"

#include <map>
#include <string>

class Logger
{
public:
    Logger() = default;
    Logger(const std::string& Name, const std::string& Context);

    virtual void Error(const std::string& Message);
    virtual void Info(const std::string& Message);
    virtual void Warning(const std::string& Message);

private:
    std::string m_Name = "";
    std::string m_Context = "";
    std::map<LogLevel, std::string> m_LogLevelNamesMap = {
        { LogLevel::ERROR, "ERROR" },
        { LogLevel::INFO, "INFO" },
        { LogLevel::WARNING, "WARNING" }
    };
    std::map<LogLevel, std::string> m_LogLevelColorsMap = {
        { LogLevel::ERROR, "\033[31m" },
        { LogLevel::INFO, "\033[35m" },
        { LogLevel::WARNING, "\033[33m" }
    };

    void Log(LogLevel LogLevel, const std::string& Message);
};
