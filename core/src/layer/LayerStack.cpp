#include "layer/LayerStack.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

// **********
// * PUBLIC *
// **********
void LayerStack::Clear()
{
    for (std::shared_ptr<Layer> Layer : m_Layers)
    {
        Layer->OnDetach();
    }

    m_Layers.clear();
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

std::vector<std::shared_ptr<Layer>>LayerStack::GetLayers() const
{
    return m_Layers;
}

int LayerStack::GetSize() const
{
    return m_Layers.size();
}

bool LayerStack::IsEmpty() const
{
    return m_Layers.empty();
}
