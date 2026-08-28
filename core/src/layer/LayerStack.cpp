#include "layer/LayerStack.h"

#include <algorithm>
#include <functional>
#include <utility>

// **********
// * PUBLIC *
// **********
void LayerStack::Clear()
{
    for (Layer* layer : m_layers)
    {
        layer->OnDetach();
    }

    m_layers.clear();
}

void LayerStack::Push(Layer* layer)
{
    if (layer == nullptr)
    {
        return;
    }

    m_layers.emplace_back(layer);
    m_layers.back()->OnAttach();
}

void LayerStack::Pop()
{
    if (m_layers.empty())
    {
        return;
    }

    m_layers.back()->OnDetach();
    m_layers.pop_back();
}

void LayerStack::Suspend(const std::string& layerId) const
{
    for (Layer* layer : m_layers)
    {
        if (layer->GetId() != layerId)
        {
            continue;
        }

        layer->Suspend();
        layer->OnSuspend();
        
        return;
    }
}

void LayerStack::Unsuspend(const std::string& layerId) const
{
    for (Layer* layer : m_layers)
    {
        if (layer->GetId() != layerId)
        {
            continue;
        }

        layer->Unsuspend();
        layer->OnUnsuspend();

        return;
    }
}

void LayerStack::Update() const
{
    for (Layer* layer : m_layers)
    {
        if (layer->IsSuspended())
        {
            continue;
        }

        layer->OnUpdate();
    }
}

void LayerStack::Render() const
{
    for (Layer* layer : m_layers)
    {
        if (layer->IsSuspended())
        {
            continue;
        }

        layer->OnRender();
    }
}

const std::vector<Layer*>& LayerStack::GetLayers() const
{
    return m_layers;
}

int LayerStack::GetSize() const
{
    return m_layers.size();
}

Layer* LayerStack::GetTop() const
{
    if (m_layers.empty())
    {
        return nullptr;
    }

    return m_layers.back();
}

bool LayerStack::IsEmpty() const
{
    return m_layers.empty();
}
