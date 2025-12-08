#include "layer/LayerStack.h"

#include <memory>
#include <vector>

// **********
// * PUBLIC *
// **********
void LayerStack::Clear()
{
    while (!m_Layers.empty())
    {
        Pop();
    }
}

bool LayerStack::IsEmpty() const
{
    return m_Layers.empty();
}

void LayerStack::Push(std::shared_ptr<Layer> Layer)
{
    m_Layers.emplace_back(Layer);
    m_Layers.back()->OnAttach();
}

void LayerStack::Pop()
{
    if (m_Layers.empty()) return;

    m_Layers.back()->OnDetach();
    m_Layers.pop_back();
}

void LayerStack::Suspend(const std::string& LayerID) const
{
    for (const std::shared_ptr<Layer>& Layer : m_Layers)
    {
        if (Layer->GetID() != LayerID) continue;

        Layer->Suspend();
        Layer->OnSuspend();
        return;
    }
}

void LayerStack::Unsuspend(const std::string& LayerID) const
{
    for (const std::shared_ptr<Layer>& Layer : m_Layers)
    {
        if (Layer->GetID() != LayerID) continue;

        Layer->Unsuspend();
        Layer->OnUnsuspend();
        return;
    }
}

void LayerStack::Update() const
{
    for (const std::shared_ptr<Layer>& Layer : m_Layers)
    {
        if (Layer->IsSuspended()) continue;
        Layer->OnUpdate();
    }
}

void LayerStack::Render() const
{
    for (const std::shared_ptr<Layer>& Layer : m_Layers)
    {
        if (Layer->IsSuspended()) continue;
        Layer->OnRender();
    }
}

int LayerStack::GetSize() const
{
    return m_Layers.size();
}
