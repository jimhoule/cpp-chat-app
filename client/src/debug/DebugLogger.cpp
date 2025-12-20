#include "debug/DebugLogger.h"

// **********
// * PUBLIC *
// **********
DebugLogger::DebugLogger(const std::string& Name, const std::string& Context, const std::shared_ptr<DebugOverlay>& DebugOverlay) : Logger(Name, Context), m_DebugOverlay(DebugOverlay)
{}

void DebugLogger::Error(const std::string& Message)
{
    ScreenMessage ErrorScreenMessage = {};
    ErrorScreenMessage.Text = Message;
    ErrorScreenMessage.Color = Rgba(255, 0, 0, 255);
    m_DebugOverlay->AddScreenMessage(ErrorScreenMessage);

    Logger::Error(Message);
}

void DebugLogger::Info(const std::string& Message)
{
    ScreenMessage InfoScreenMessage = {};
    InfoScreenMessage.Text = Message;
    InfoScreenMessage.Color = Rgba(255, 0, 255, 255);
    m_DebugOverlay->AddScreenMessage(InfoScreenMessage);

    Logger::Info(Message);
}

void DebugLogger::Warning(const std::string& Message)
{
    ScreenMessage WarningScreenMessage = {};
    WarningScreenMessage.Text = Message;
    WarningScreenMessage.Color = Rgba(255, 255, 0, 255);
    m_DebugOverlay->AddScreenMessage(WarningScreenMessage);

    Logger::Warning(Message);
}
