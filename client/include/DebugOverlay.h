#pragma once

#include "Gui.h"
#include "layer/LayerStack.h"

#include <chrono>
#include <functional>
#include <memory>
#include <string>

class DebugOverlay
{
  public:
	DebugOverlay(const Gui &Gui, const std::shared_ptr<LayerStack> &LayerStack);

	void Render();

  private:
	Gui m_Gui = {};
	std::shared_ptr<LayerStack> m_LayerStack = nullptr;

	std::chrono::steady_clock::time_point m_PreviousUpdateTime = std::chrono::steady_clock::now();
	float m_Fps = 0.0f;
	bool m_IsVisible = false;
};
