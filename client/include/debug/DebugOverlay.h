#pragma once

#include "Gui.h"

#include <chrono>
#include <functional>
#include <deque>

// Forward declarations
class LayerStack;

struct ScreenMessage {
    std::string text;
    Rgba color = Rgba(255, 255, 255, 255);
};

class DebugOverlay
{
  public:
	DebugOverlay(const Gui& gui, LayerStack& layerStack);

    void AddScreenMessage(const ScreenMessage& screenMessage);
	void Render();

  private:
	Gui m_gui = {};
	LayerStack& m_layerStack;
    std::deque<ScreenMessage> m_screenMessages = {};
	std::chrono::steady_clock::time_point m_previousUpdateTime = std::chrono::steady_clock::now();
	float m_fps = 0.0f;
	bool m_isVisible = false;

    float CalculateFps() const;
};
