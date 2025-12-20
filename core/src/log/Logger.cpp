#include "log/Logger.h"

#include <ctime>
#include <iomanip>
#include <iostream>

// **********
// * PUBLIC *
// **********
Logger::Logger(const std::string& Name, const std::string& Context) : m_Name(Name), m_Context(Context)
{}

void Logger::Error(const std::string& Message)
{
    Log(LogLevel::ERROR, Message);
}

void Logger::Info(const std::string& Message)
{
    Log(LogLevel::INFO, Message);
}

void Logger::Warning(const std::string& Message)
{
    Log(LogLevel::WARNING, Message);
}

// ***********
// * PRIVATE *
// ***********
void Logger::Log(LogLevel LogLevel, const std::string& Message)
{
    const std::string& GreentColor = "\033[32m";
    const std::string& WhitetColor = "\033[0m";

    time_t Now = std::time(0);
    std::tm* NowDate = std::localtime(&Now);

    std::cout << m_LogLevelColorsMap[LogLevel] << " [" << m_Name << "] "
              << WhitetColor << std::put_time(NowDate, "%c") << "    "
              << m_LogLevelColorsMap[LogLevel] <<  m_LogLevelNamesMap[LogLevel]
              << GreentColor << " [" << m_Context << "] "
              << m_LogLevelColorsMap[LogLevel] << Message
              << WhitetColor << std::endl;
}
