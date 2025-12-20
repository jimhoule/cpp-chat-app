#pragma once

#include "debug/DebugOverlay.h"
#include "log/Logger.h"

class DebugLogger : public Logger
{
public:
    DebugLogger(const std::string& Name, const std::string& Context, const std::shared_ptr<DebugOverlay>& DebugOverlay);

    void Error(const std::string& Message) override;
    void Info(const std::string& Message) override;
    void Warning(const std::string& Message) override;

private:
    std::shared_ptr<DebugOverlay> m_DebugOverlay = nullptr;
};
