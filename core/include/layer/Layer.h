#pragma once

#include <string>

// Forward declarations
class Logger;

class Layer
{
public:
    Layer(const std::string& id, Logger& logger);
    virtual ~Layer() = default;

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnUpdate();
    virtual void OnSuspend();
    virtual void OnUnsuspend();
    virtual void OnRender() = 0;

    const std::string& GetId() const;
    bool IsSuspended() const;
    void Suspend();
    void Unsuspend();

protected:
    Logger& m_logger;

private:
    std::string m_id = "";
    bool m_isSuspended = false;
};
