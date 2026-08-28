#include "debug/DebugLogger.h"

#include "debug/DebugOverlay.h"

// **********
// * PUBLIC *
// **********
DebugLogger::DebugLogger(const std::string& name, const std::string& context, DebugOverlay& debugOverlay)
    : Logger(name, context)
    , m_debugOverlay(debugOverlay)
{}

void DebugLogger::Error(const std::string& message)
{
    ScreenMessage errorScreenMessage = {};
    errorScreenMessage.text = message;
    errorScreenMessage.color = Rgba(255, 0, 0, 255);
    m_debugOverlay.AddScreenMessage(errorScreenMessage);

    Logger::Error(message);
}

void DebugLogger::Info(const std::string& message)
{
    ScreenMessage infoScreenMessage = {};
    infoScreenMessage.text = message;
    infoScreenMessage.color = Rgba(255, 0, 255, 255);
    m_debugOverlay.AddScreenMessage(infoScreenMessage);

    Logger::Info(message);
}

void DebugLogger::Warning(const std::string& message)
{
    ScreenMessage warningScreenMessage = {};
    warningScreenMessage.text = message;
    warningScreenMessage.color = Rgba(255, 255, 0, 255);
    m_debugOverlay.AddScreenMessage(warningScreenMessage);

    Logger::Warning(message);
}
