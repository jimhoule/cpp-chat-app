#include "Gui.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// **********
// * PUBLIC *
// **********
void Gui::Init(GLFWwindow* GlfwWindow) const
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& Io = ImGui::GetIO();
    (void)Io;
    Io.Fonts->AddFontFromFileTTF("../../assets/Audiowide-Regular.ttf");

     // Setup Dear ImGui style
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(GlfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}

void Gui::Destroy() const
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::Render() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::Clear() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::DrawButton(Button& Button) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, Button.BorderSize);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(Button.Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Button.CornerRounding);
    ImGui::PushStyleColor(ImGuiCol_Button, ToImVec4(Button.Color.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec4(Button.ColorActive.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec4(Button.ColorHovered.ToVector4()));

    if (Button.IsDisabled)  ImGui::BeginDisabled();

    if (ImGui::Button(Button.Label.c_str(), ImVec2(Button.Size.X, Button.Size.Y))) Button.OnClick();
    if (ImGui::IsItemHovered() && !!Button.OnHover) Button.OnHover();

    if (Button.IsDisabled)  ImGui::EndDisabled();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
}

void Gui::DrawContainer(Container& Container) const
{
    ImGuiChildFlags Flags = ImGuiChildFlags_AlwaysUseWindowPadding;
    if (Container.IsAutoResizableY) Flags |= ImGuiChildFlags_AutoResizeY;
    if (Container.IsAutoResizableX) Flags |= ImGuiChildFlags_AutoResizeX;

    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, Container.BorderSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(Container.Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, Container.CornerRounding);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ToImVec4(Container.BgColor.ToVector4()));

    if (ImGui::BeginChild(Container.ID.c_str(), ToImVec2(Container.Size), Flags)) Container.DrawContent();
    ImGui::EndChild();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(1);
}

void Gui::DrawImage(const Image& Image) const
{
    // NOTE: Uses window drawlist instead of forground drawlist to avoid image being visible when scrolling out of sight
    ImDrawList* WindowDrawList = ImGui::GetWindowDrawList();

    ImVec2 Position = ImGui::GetCursorScreenPos();
    ImVec2 Size = ImVec2(Position.x + Image.Size.X, Position.y + Image.Size.Y);
    ImVec2 UvPositionStart = ImVec2(0.0f, 0.0f);
    ImVec2 UvPositionEnd = ImVec2(1.0f, 1.0f);
    ImU32 TintColor = IM_COL32_WHITE;

    WindowDrawList->AddImageRounded(
        Image.TextureID,
        Position,
        Size,
        UvPositionStart,
        UvPositionEnd,
        TintColor,
        Image.CornerRounding,
        ImDrawFlags_RoundCornersAll
    );
}

 void Gui::DrawText(const Text& Text) const
 {
    ImGui::TextUnformatted(Text.Value.c_str());
 }

void Gui::DrawTextInputMultiline(std::string& Value, TextInput& TextInput) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, TextInput.BorderSize);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(TextInput.Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, TextInput.CornerRounding);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ToImVec4(TextInput.BgColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(TextInput.TextColor.ToVector4()));

    // NOTE: ## prefix tells ImGui to use the string for internal ID generation but not to display it as a visible label
    const std::string& ID = "##" + TextInput.ID;
    ImGui::InputTextMultiline(ID.c_str(), &Value, ToImVec2(TextInput.Size));

    // Draws placeholder on top of Text input multiline
    if (!TextInput.Placeholder.empty() && Value.size() == 0)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 FramePadding = style.FramePadding;

        ImDrawList* ForegroundDrawList = ImGui::GetForegroundDrawList();
        ImVec2 BoundingBoxMinPosition = ImGui::GetItemRectMin();

        ImVec2 PlaceholderPosition(
            BoundingBoxMinPosition.x + FramePadding.x,
            BoundingBoxMinPosition.y + FramePadding.y
        );
        ImU32 PlaceholderColor = IM_COL32(
            TextInput.PlaceholderColor.R,
            TextInput.PlaceholderColor.G,
            TextInput.PlaceholderColor.B,
            TextInput.PlaceholderColor.A
        );

        ForegroundDrawList->AddText(PlaceholderPosition, PlaceholderColor, TextInput.Placeholder.c_str());
    }

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void Gui::DrawTreeNode(const TreeNode& Node) const
{
    if (ImGui::TreeNode(Node.Name.c_str()))
    {
        for (const TreeNode& Child : Node.Children)
        {
            DrawTreeNode(Child);
        }
        ImGui::TreePop();
    }
}

void Gui::DrawWindow(Window& Window) const
{
    bool IsOpen = true;

    ImGuiWindowFlags Flags = ImGuiWindowFlags_None;
    if (!Window.CanSaveSettigs) Flags |= ImGuiWindowFlags_NoSavedSettings;
    if (!Window.IsTitlebarVisible) Flags |= ImGuiWindowFlags_NoTitleBar;
    if (!Window.IsScrollbarVisible) Flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    if (!Window.IsResizable) Flags |= ImGuiWindowFlags_NoResize;
    if (!Window.IsCollapsible) Flags |= ImGuiWindowFlags_NoCollapse;
    if (!Window.IsMovable) Flags |= ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(Window.Padding));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ToImVec4(Window.BgColor.ToVector4()));

    ImGui::SetNextWindowPos(ToImVec2(Window.Position));
    ImGui::SetNextWindowSize(ToImVec2(Window.Size));
    if (ImGui::Begin(Window.Name.c_str(), &IsOpen, Flags)) Window.DrawContent();
    ImGui::End();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(1);
}

void Gui::AlignCenter(Vector2 ElementSize) const
{
    const Vector2 AVAILABLE_SPACE = GetAvailableSpace();
    const Vector2 OFFSET = (AVAILABLE_SPACE - ElementSize) * 0.5f;
    SetPosition(GetPosition() + OFFSET);
}

void Gui::AlignCenterX(float ElementWidth) const
{
    const Vector2 AVAILABLE_SPACE = GetAvailableSpace();
    const float OFFSET_X = (AVAILABLE_SPACE.X - ElementWidth) * 0.5f;
    SetPositionX(GetPositionX() + OFFSET_X);
}

void Gui::AlignCenterY(float ElementHeigth) const
{
    const Vector2 AVAILABLE_SPACE = GetAvailableSpace();
    const float OFFSET_Y = (AVAILABLE_SPACE.Y - ElementHeigth) * 0.5f;
    SetPositionY(GetPositionY() + OFFSET_Y);
}

void Gui::DisplayInline() const
{
    ImGui::SameLine();
}

void Gui::ScrollToY(float Y) const
{
    ImGui::SetScrollHereY(Y);
}

void Gui::ScrollToX(float X) const
{
    ImGui::SetScrollHereX(X);
}

float Gui::GetScrollPositionY() const
{
    return ImGui::GetScrollY();
}

float Gui::GetScrollPositionX() const
{
    return ImGui::GetScrollX();
}

float Gui::GetMaxScrollPositionY() const
{
    return ImGui::GetScrollMaxY();
}

float Gui::GetMaxScrollPositionX() const
{
    return ImGui::GetScrollMaxX();
}

const Vector2 Gui::GetAvailableSpace() const
{
    return ToVector2(ImGui::GetContentRegionAvail());
}

const Vector2 Gui::GetPosition() const
{
    return ToVector2(ImGui::GetCursorPos());
}

float Gui::GetPositionX() const
{
    return ImGui::GetCursorPosX();
}

float Gui::GetPositionY() const
{
    return ImGui::GetCursorPosY();
}

void Gui::SetPosition(Vector2 Position) const
{
    ImGui::SetCursorPos(ToImVec2(Position));
}

void Gui::SetPositionX(float X) const
{
    ImGui::SetCursorPosX(X);
}

void Gui::SetPositionY(float Y) const
{
    ImGui::SetCursorPosY(Y);
}

// ***********
// * PRIVATE *
// ***********
const Vector2 Gui::ToVector2(const ImVec2& ImguiVec2) const
{
    return Vector2(ImguiVec2.x, ImguiVec2.y);
}

const Vector4 Gui::ToVector4(const ImVec4& ImguiVec4) const
{
    return Vector4(ImguiVec4.x, ImguiVec4.y, ImguiVec4.z, ImguiVec4.w);
}

const ImVec2 Gui::ToImVec2(const Vector2& Vector2) const
{
    return ImVec2(Vector2.X, Vector2.Y);
}

const ImVec4 Gui::ToImVec4(const Vector4& Vector4) const
{
    return ImVec4(Vector4.X, Vector4.Y, Vector4.Z, Vector4.W);
}
