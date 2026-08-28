#pragma once

#include "log/Logger.h"

// Forward declarations
class DebugOverlay;

class DebugLogger : public Logger
{
public:
    DebugLogger(const std::string& name, const std::string& context, DebugOverlay& debugOverlay);

    void Error(const std::string& message) override;
    void Info(const std::string& message) override;
    void Warning(const std::string& message) override;

private:
    DebugOverlay& m_debugOverlay;
};
