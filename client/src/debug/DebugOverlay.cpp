#include "debug/DebugOverlay.h"

#include "layer/LayerStack.h"

// **********
// * PUBLIC *
// **********
DebugOverlay::DebugOverlay(const Gui& gui, LayerStack& layerStack)
 : m_gui(gui)
 , m_layerStack(layerStack)
 , m_fps(CalculateFps())
{}

void DebugOverlay::AddScreenMessage(const ScreenMessage& ScreenMessage)
{
    // NOTE: Limits screen messages to 10
    if (m_screenMessages.size() == 10) m_screenMessages.pop_front();

    m_screenMessages.push_back(ScreenMessage);
}

void DebugOverlay::Render()
{
    if (ImGui::IsKeyPressed(ImGuiKey_Q)) m_isVisible = !m_isVisible;
    if (!m_isVisible) return;

    Gui::RawWindow debugOverlayRawWindow = {};
    debugOverlayRawWindow.minCornerPosition = Vector2(0.0f, 0.0f);
    debugOverlayRawWindow.maxCornerPosition = m_gui.GetViewportSize();
    debugOverlayRawWindow.bgColor = Rgba(0, 0, 0, 100);
    debugOverlayRawWindow.cornerRounding = 0.0f;
    debugOverlayRawWindow.DrawContent = [this]() {
        // Fps
        std::chrono::steady_clock::time_point currentUpdateTime = std::chrono::steady_clock::now();
        std::chrono::duration updateTimeInterval = std::chrono::duration_cast<std::chrono::seconds>(currentUpdateTime - m_previousUpdateTime);
        // NOTE: Limits fps text update to 4 times per seconds
        if (updateTimeInterval.count() >= 0.25f)
        {
            m_fps = CalculateFps();
            m_previousUpdateTime = currentUpdateTime;
        }

        Gui::RawText fpsRawText = {};
        fpsRawText.value = "FPS: " + std::to_string(m_fps);
        fpsRawText.position = Vector2(0.0f, 0.0f);
        fpsRawText.color = Rgba(255, 255, 255, 255);

        m_gui.DrawRawText(fpsRawText);

        // Layerstack size
        Gui::RawText layerStackSizeRawText = {};
        layerStackSizeRawText.value = "LayerStack size: " + std::to_string(m_layerStack.GetSize());
        layerStackSizeRawText.position = Vector2(0.0f, 40.0f);
        layerStackSizeRawText.color = Rgba(255, 255, 255, 255);

        m_gui.DrawRawText(layerStackSizeRawText);

        // Layers
        float layerRawTextPositionY = 60.0f;
        for (int Index = 0; Index < m_layerStack.GetLayers().size(); Index++)
        {
            Layer* Layer = m_layerStack.GetLayers()[Index];

            Gui::RawText layerRawText = {};
            layerRawText.value = "Index " + std::to_string(Index) + ": " + Layer->GetId();
            layerRawText.position = Vector2(0.0f, layerRawTextPositionY);
            layerRawText.color = Rgba(255, 255, 255, 255);

            m_gui.DrawRawText(layerRawText);

            layerRawTextPositionY += 20.0f;
        }

        // Screen messages
        float screenMessageRawTextPositionY = layerRawTextPositionY + 20.0f;
        for (const ScreenMessage& screenMessage : m_screenMessages)
        {
            Gui::RawText screenMessageRawText = {};
            screenMessageRawText.value = screenMessage.text;
            screenMessageRawText.position = Vector2(0.0f, screenMessageRawTextPositionY);
            screenMessageRawText.color = screenMessage.color;

            m_gui.DrawRawText(screenMessageRawText);

            screenMessageRawTextPositionY += 20.0f;
        }
    };

    m_gui.DrawRawWindow(debugOverlayRawWindow);
}


// ***********
// * PRIVATE *
// ***********
float DebugOverlay::CalculateFps() const
{
    return 1.0f / m_gui.GetDeltaTime();
}
