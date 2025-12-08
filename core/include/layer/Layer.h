#pragma once

#include <string>

class Layer
{
public:
    Layer(const std::string& ID);
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
private:
    std::string m_ID = "";
    bool m_IsSuspended = false;
};
