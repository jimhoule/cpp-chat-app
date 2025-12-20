#pragma once

#include "layer/Layer.h"

#include <vector>

class LayerStack
{
public:
    void Clear();
    void Pop();
    void Push(std::shared_ptr<Layer> Layer);
    void Suspend(const std::string& LayerID) const;
    void Unsuspend(const std::string& LayerID) const;
    void Update() const;
    void Render() const;
    std::vector<std::shared_ptr<Layer>> GetLayers() const;
    int GetSize() const;
    bool IsEmpty() const;

private:
    std::vector<std::shared_ptr<Layer>> m_Layers = {};
};
