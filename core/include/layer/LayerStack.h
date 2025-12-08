#pragma once

#include "layer/Layer.h"

#include <memory>
#include <vector>

class LayerStack
{
public:
    void Clear();
    bool IsEmpty() const;
    void Push(std::shared_ptr<Layer> Layer);
    void Pop();
    void Suspend(const std::string& LayerID) const;
    void Unsuspend(const std::string& LayerID) const;
    void Update() const;
    void Render() const;

    int GetSize() const;

private:
    std::vector<std::shared_ptr<Layer>> m_Layers = {};
};
