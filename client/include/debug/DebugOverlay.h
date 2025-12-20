#pragma once

#include "Gui.h"
#include "layer/LayerStack.h"

#include <chrono>
#include <functional>
#include <deque>

struct ScreenMessage {
    std::string Text;
    Rgba Color = Rgba(255, 255, 255, 255);
};

class DebugOverlay
{
  public:
    DebugOverlay() = default;
	DebugOverlay(const Gui &Gui, const std::shared_ptr<LayerStack>& LayerStack);

    void AddScreenMessage(const ScreenMessage& ScreenMessage);
	void Render();

  private:
	Gui m_Gui = {};
	std::shared_ptr<LayerStack> m_LayerStack = nullptr;
    std::deque<ScreenMessage> m_ScreenMessages = {};
	std::chrono::steady_clock::time_point m_PreviousUpdateTime = std::chrono::steady_clock::now();
	float m_Fps = 0.0f;
	bool m_IsVisible = false;
};
