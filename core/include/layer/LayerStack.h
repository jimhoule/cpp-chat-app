#pragma once

#include "layer/Layer.h"

#include <vector>

class LayerStack
{
public:
    LayerStack() = default;

    void Clear();
    void Pop();
    void Push(Layer* layer);
    void Suspend(const std::string& layerId) const;
    void Unsuspend(const std::string& layerId) const;
    void Update() const;
    void Render() const;
    const std::vector<Layer*>& GetLayers() const;
    int GetSize() const;
    Layer* GetTop() const;
    bool IsEmpty() const;

private:
    std::vector<Layer*> m_layers = {};
};
