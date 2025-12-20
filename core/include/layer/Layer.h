#pragma once

#include "log/Logger.h"

#include <memory>

class Layer
{
public:
    Layer(const std::string& ID, const std::shared_ptr<Logger>& Logger);
    virtual ~Layer() = default;

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnUpdate();
    virtual void OnSuspend();
    virtual void OnUnsuspend();
    virtual void OnRender() = 0;

    std::string& GetID();
    bool IsSuspended() const;
    void Suspend();
    void Unsuspend();

protected:
    std::shared_ptr<Logger> m_Logger = nullptr;

private:
    std::string m_ID = "";
    bool m_IsSuspended = false;
};
