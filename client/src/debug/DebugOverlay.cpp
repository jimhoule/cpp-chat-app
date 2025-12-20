#include "debug/DebugOverlay.h"

// **********
// * PUBLIC *
// **********
DebugOverlay::DebugOverlay(const Gui& Gui, const std::shared_ptr<LayerStack>& LayerStack) : m_Gui(Gui), m_LayerStack(LayerStack)
{
    ImGuiIO& Io = ImGui::GetIO();
    (void)Io;
    m_Fps = 1.0f / Io.DeltaTime;
}

void DebugOverlay::AddScreenMessage(const ScreenMessage& ScreenMessage)
{
    // NOTE: Limits screen messages to 10
    if (m_ScreenMessages.size() == 10) m_ScreenMessages.pop_front();

    m_ScreenMessages.push_back(ScreenMessage);
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
        RawText LayerStackSizeRawText = {};
        LayerStackSizeRawText.Value = "LayerStack size: " + std::to_string(m_LayerStack->GetSize());
        LayerStackSizeRawText.Position = Vector2(0.0f, 40.0f);
        LayerStackSizeRawText.Color = Rgba(255, 255, 255, 255);

        m_Gui.DrawRawText(LayerStackSizeRawText);

        // Layers
        float LayerRawTextPositionY = 60.0f;
        for (int Index = 0; Index < m_LayerStack->GetLayers().size(); Index++)
        {
            std::shared_ptr<Layer> Layer = m_LayerStack->GetLayers()[Index];

            RawText LayerRawText = {};
            LayerRawText.Value = "Index " + std::to_string(Index) + ": " + Layer->GetID();
            LayerRawText.Position = Vector2(0.0f, LayerRawTextPositionY);
            LayerRawText.Color = Rgba(255, 255, 255, 255);

            m_Gui.DrawRawText(LayerRawText);

            LayerRawTextPositionY += 20.0f;
        }

        // Screen messages
        float ScreenMessageRawTextPositionY = LayerRawTextPositionY + 20.0f;
        for (const ScreenMessage& ScreenMessage : m_ScreenMessages)
        {
            RawText ScreenMessageRawText = {};
            ScreenMessageRawText.Value = ScreenMessage.Text;
            ScreenMessageRawText.Position = Vector2(0.0f, ScreenMessageRawTextPositionY);
            ScreenMessageRawText.Color = ScreenMessage.Color;

            m_Gui.DrawRawText(ScreenMessageRawText);

            ScreenMessageRawTextPositionY += 20.0f;
        }
    };

    m_Gui.DrawRawWindow(DebugOverlayRawWindow);
}
