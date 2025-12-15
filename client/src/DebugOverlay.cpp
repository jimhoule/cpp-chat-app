#include "DebugOverlay.h"

// **********
// * PUBLIC *
// **********
DebugOverlay::DebugOverlay(const Gui& Gui, const std::shared_ptr<LayerStack>& LayerStack) : m_Gui(Gui), m_LayerStack(LayerStack)
{
    ImGuiIO& Io = ImGui::GetIO();
    (void)Io;
    m_Fps = 1.0f / Io.DeltaTime;
}

void DebugOverlay::Render()
{
    if (ImGui::IsKeyPressed(ImGuiKey_Q)) m_IsVisible = !m_IsVisible;
    if (!m_IsVisible) return;

    RawWindow DebugOverlayRawWindow = {};
    DebugOverlayRawWindow.MinCornerPosition = Vector2(0.0f, 0.0f);
    DebugOverlayRawWindow.MaxCornerPosition = m_Gui.GetViewportSize();
    DebugOverlayRawWindow.BgColor = Rgba(0, 0, 0, 100);
    DebugOverlayRawWindow.CornerRounding = 0.0f;
    DebugOverlayRawWindow.DrawContent = [this]() {
        // Fps
        std::chrono::steady_clock::time_point CurrentUpdateTime = std::chrono::steady_clock::now();
        std::chrono::duration UpdateTimeInterval = std::chrono::duration_cast<std::chrono::seconds>(CurrentUpdateTime - m_PreviousUpdateTime);
        // NOTE: Limits fps text update to 4 times per seconds
        if (UpdateTimeInterval.count() >= 0.25f)
        {
            ImGuiIO& Io = ImGui::GetIO();
            (void)Io;
            m_Fps = 1.0f / Io.DeltaTime;
            m_PreviousUpdateTime = CurrentUpdateTime;
        }

        RawText FpsRawText = {};
        FpsRawText.Value = "FPS: " + std::to_string(m_Fps);
        FpsRawText.Position = Vector2(0.0f, 0.0f);
        FpsRawText.Color = Rgba(255, 255, 255, 255);

        m_Gui.DrawRawText(FpsRawText);

        // Layerstack size
        RawText LayerStackSizeText = {};
        FpsRawText.Value = "LayerStack size: " + std::to_string(m_LayerStack->GetSize());
        FpsRawText.Position = Vector2(0.0f, 40.0f);
        FpsRawText.Color = Rgba(255, 255, 255, 255);

        m_Gui.DrawRawText(FpsRawText);

        // Layers
        float TextPositionY = 60.0f;
        for (int Index = 0; Index < m_LayerStack->GetLayers().size(); Index++)
        {
            std::shared_ptr<Layer> Layer = m_LayerStack->GetLayers()[Index];

            RawText LayerText = {};
            LayerText.Value = "Index " + std::to_string(Index) + ": " + Layer->GetID();
            LayerText.Position = Vector2(0.0f, TextPositionY);
            LayerText.Color = Rgba(255, 255, 255, 255);

            m_Gui.DrawRawText(LayerText);

            TextPositionY += 20.0f;
        }
    };

    m_Gui.DrawRawWindow(DebugOverlayRawWindow);
}

// ***********
// * PRIVATE *
// ***********
